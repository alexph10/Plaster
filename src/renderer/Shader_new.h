#pragma once

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <glad/glad.h>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>


// Forward declarations
class ShaderCompiler;
class UniformCache;

// High-performance uniform caching
class UniformCache {
public:
  GLint GetUniformLocation(GLuint program, const std::string &name) const;
  void CacheUniform(const std::string &name, GLint location);
  void ClearCache();

private:
  mutable std::unordered_map<std::string, GLint> m_cache;
  mutable std::mutex m_mutex;
};

// Advanced shader compiler with hot-reloading
class ShaderCompiler {
public:
  bool CompileShader(const std::string &source, GLenum type, GLuint &shader);
  bool LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint &program);
  bool ValidateProgram(GLuint program);
  void EnableDebugOutput();

private:
  void CheckCompileErrors(GLuint shader, const std::string &type);
  void CheckLinkErrors(GLuint program);
  std::atomic<bool> m_debugEnabled{false};
};

class Shader {
public:
  Shader();
  ~Shader();

  // Async loading and hot-reloading
  bool LoadFromFilesAsync(const std::string &vertPath,
                          const std::string &fragPath);
  void CheckAndRecompile();

  // Core functionality
  void Use() const;
  void Destroy();

  // Uniform setters with caching
  void SetBool(const std::string &name, bool value);
  void SetInt(const std::string &name, int value);
  void SetFloat(const std::string &name, float value);
  void SetVec2(const std::string &name, float x, float y);
  void SetVec3(const std::string &name, float x, float y, float z);
  void SetVec4(const std::string &name, float x, float y, float z, float w);
  void SetMat4(const std::string &name, const float *value);

  // Getters
  GLuint GetProgram() const { return m_program; }
  bool IsValid() const { return m_program != 0; }

  // File watching for hot-reload
  void EnableHotReload(bool enable = true) { m_hotReload = enable; }

private:
  GLuint m_program = 0;
  std::string m_vertexPath;
  std::string m_fragmentPath;

  // File watching
  std::filesystem::file_time_type m_lastVertWrite;
  std::filesystem::file_time_type m_lastFragWrite;
  bool m_hotReload = false;

  // Performance optimizations
  mutable UniformCache m_uniformCache;
  static std::unique_ptr<ShaderCompiler> s_compiler;

  // Helper methods
  void CacheLocation(const std::string &name, GLint location);
  GLint GetUniformLocation(const std::string &name);
  bool LoadShaderFile(const std::string &path, std::string &content);
};