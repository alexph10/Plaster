#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

std::unique_ptr<ShaderCompiler> Shader::s_compiler = nullptr;

Shader::Shader() {
    if (!s_compiler) s_compiler = std::make_unique<ShaderCompiler>();
}
Shader::~Shader() {
    Destroy();
}

bool Shader::LoadFromFilesAsync(const std::string& vertPath, const std::string& fragPath) {
    m_vertPath = vertPath;
    m_fragPath = fragPath;
    m_compileFuture = s_compiler->CompileAsync(ReadFile(vertPath), ReadFile(fragPath));
    return true;
}

void Shader::CheckAndRecompile() {
    if (m_compileFuture.valid()) {
        auto status = m_compileFuture.wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready) {
            GLuint programID = m_compileFuture.get();
            if (programID != 0) {
                Destroy();
                m_programID = programID;
            }
        }
    }
    // Hot reload based on file modification time
    auto vertTime = std::filesystem::last_write_time(m_vertPath);
    auto fragTime = std::filesystem::last_write_time(m_fragPath);
    if (vertTime != m_vertLastWrite || fragTime != m_fragLastWrite) {
        LoadFromFilesAsync(m_vertPath.string(), m_fragPath.string());
        m_vertLastWrite = vertTime;
        m_fragLastWrite = fragTime;
    }
}

void Shader::Use() const {
    glUseProgram(m_programID);
    CheckAndRecompile();
}

void Shader::Destroy() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
        m_programID = 0;
    }
}

// Uniform caching
void Shader::CacheLocation(const std::string& name, GLint location) {
    m_uniformLocations[name] = location;
}
GLint Shader::GetLocation(const std::string& name) {
    auto it = m_uniformLocations.find(name);
    if (it != m_uniformLocations.end()) return it->second;
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    CacheLocation(name, loc);
    return loc;
}

void Shader::SetUniform(const std::string& name, float value) const {
    GLint loc = m_cache.GetLocation(name);
    if (loc != -1) glUniform1f(loc, value);
}
void Shader::SetUniform(const std::string& name, int value) const {
    GLint loc = m_cache.GetLocation(name);
    if (loc != -1) glUniform1i(loc, value);
}
void Shader::SetUniform(const std::string& name, const glm::vec2& v) const {
    GLint loc = m_cache.GetLocation(name);
    if (loc != -1) glUniform2fv(loc, 1, &v[0]);
}
void Shader::SetUniform(const std::string& name, const glm::vec3& v) const {
    GLint loc = m_cache.GetLocation(name);
    if (loc != -1) glUniform3fv(loc, 1, &v[0]);
}
void Shader::SetUniform(const std::string& name, const glm::vec4& v) const {
    GLint loc = m_cache.GetLocation(name);
    if (loc != -1) glUniform4fv(loc, 1, &v[0]);
}
void Shader::SetUniform(const std::string& name, const glm::mat4& m) const {
    GLint loc = m_cache.GetLocation(name);
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

std::string Shader::ReadFile(const std::filesystem::path& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to read shader: " << filepath << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}



