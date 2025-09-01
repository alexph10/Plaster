#include "Shader.h"
#include <GL/gl.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <chrono>

// Thread-safe queue implementation
template<typename T>
class ThreadSafeQueue {
public:
    void push(T item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(item));
        m_condition.notify_one();
    }
    
    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) return false;
        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }
    
    void wait_and_pop(T& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this] { return !m_queue.empty(); });
        item = std::move(m_queue.front());
        m_queue.pop();
    }
    
private:
    mutable std::mutex m_mutex;
    std::queue<T> m_queue;
    std::condition_variable m_condition;
};

// Static compiler instance
std::unique_ptr<ShaderCompiler> Shader::s_compiler = nullptr;

// High-performance uniform caching with hash-based lookup
GLint UniformCache::GetUniformLocation(GLuint program, const std::string& name) const {
    // Use FNV-1a hash for fast string hashing
    uint64_t hash = 14695981039346656037ULL;
    for (char c : name) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    hash ^= static_cast<uint64_t>(program) << 32;
    
    auto it = m_uniformCache.find(hash);
    if (it != m_uniformCache.end()) {
        it->second.frameLastUsed.store(m_currentFrame.load());
        return it->second.location;
    }
    
    // Cache miss - query OpenGL
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location != -1) {
        UniformInfo info;
        info.location = location;
        info.frameLastUsed.store(m_currentFrame.load());
        
        // Query uniform type and size for validation
        GLchar uniformName[256];
        glGetActiveUniform(program, location, 256, nullptr, &info.size, &info.type, uniformName);
        
        m_uniformCache[hash] = info;
    }
    
    return location;
}

void UniformCache::CleanupUnusedUniforms(uint64_t maxAge) const {
    uint64_t currentFrame = m_currentFrame.load();
    
    auto it = m_uniformCache.begin();
    while (it != m_uniformCache.end()) {
        if (currentFrame - it->second.frameLastUsed.load() > maxAge) {
            it = m_uniformCache.erase(it);
        } else {
            ++it;
        }
    }
}

// Async shader compilation
ShaderCompiler::ShaderCompiler() 
    : m_compilationThread(&ShaderCompiler::CompilationThreadFunction, this) {
}

ShaderCompiler::~ShaderCompiler() {
    m_shutdown.store(true);
    
    // Add dummy task to wake up thread
    auto dummy = std::make_unique<CompilationTask>();
    dummy->cancelled.store(true);
    m_compilationQueue.push(std::move(dummy));
    
    if (m_compilationThread.joinable()) {
        m_compilationThread.join();
    }
}

std::future<GLuint> ShaderCompiler::CompileAsync(const std::string& vertexSrc, 
                                               const std::string& fragmentSrc,
                                               const std::string& computeSrc) {
    auto task = std::make_unique<CompilationTask>();
    task->vertexSource = vertexSrc;
    task->fragmentSource = fragmentSrc;
    task->computeSource = computeSrc;
    
    auto future = task->promise.get_future();
    m_compilationQueue.push(std::move(task));
    
    return future;
}

void ShaderCompiler::CompilationThreadFunction() {
    // Create separate OpenGL context for compilation thread
    // This requires platform-specific code (omitted for brevity)
    
    while (!m_shutdown.load()) {
        std::unique_ptr<CompilationTask> task;
        m_compilationQueue.wait_and_pop(task);
        
        if (task->cancelled.load() || m_shutdown.load()) {
            task->promise.set_value(0);
            continue;
        }
        
        GLuint program = CompileShaderProgram(task->vertexSource, 
                                            task->fragmentSource, 
                                            task->computeSource);
        task->promise.set_value(program);
    }
}

template<typename T>
void Shader::SetUniformCachedImpl(const std::string& name, const T& value) const {
    auto cacheIt = m_uniformValueCache.find(name);
    if (cacheIt != m_uniformValueCache.end()) {
        try {
            if (std::any_cast<T>(cacheIt->second) == value) {
                return; // Value unchanged, skip GPU call
            }
        } catch (const std::bad_any_cast&) {
            // Type mismatch, continue with update
        }
    }
    
    GLint location = m_uniformCache.GetUniformLocation(m_programID, name);
    if (location == -1) return;
    
    // Specialized setting based on type
    if constexpr (std::is_same_v<T, float>) {
        glUniform1f(location, value);
    } else if constexpr (std::is_same_v<T, int>) {
        glUniform1i(location, value);
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
        glUniform2fv(location, 1, &value[0]);
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
        glUniform3fv(location, 1, &value);
    } else if constexpr (std::is_same_v<T, glm::vec4>) {
        glUniform4fv(location, 1, &value);
    } else if constexpr (std::is_same_v<T, glm::mat4>) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value);
    }
    
    m_uniformValueCache[name] = value;
}

void Shader::SetFloatCached(const std::string& name, float value) const {
    SetUniformCachedImpl(name, value);
}

void Shader::SetIntCached(const std::string& name, int value) const {
    SetUniformCachedImpl(name, value);
}

UniformBuffer::UniformBuffer(size_t size, GLenum usage) 
    : m_size(size), m_persistentMapping(nullptr) {
    glGenBuffers(1, &m_bufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, usage);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer() {
    if (m_persistentMapping) {
        UnmapPersistent();
    }
    if (m_bufferID) {
        glDeleteBuffers(1, &m_bufferID);
    }
}

void* UniformBuffer::MapPersistent(GLbitfield access) {
    if (m_persistentMapping) return m_persistentMapping;
    
    glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
    m_persistentMapping = glMapBufferRange(GL_UNIFORM_BUFFER, 0, m_size, access);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    return m_persistentMapping;
}

void UniformBuffer::UpdateData(const void* data, size_t size, size_t offset) {
    if (m_persistentMapping) {

        std::memcpy(static_cast<char*>(m_persistentMapping) + offset, data, size);
    } else {
        glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}




