#include "ShaderCompiler.h"
#include <cmath>
#include <shaderc/shaderc.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Plaster() {

ShaderCompiler::ShaderCompiler(){
}

ShaderCompiler::~ShaderCompiler(){
}

bool ShaderCompiler::compileFromSource(
  const std::string& source,
  ShaderStage stage,
  const std::string& entryPoint, 
  std::vector<uint32_t>& spirvOut
) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  options.SetOptimizationLevel(shaderc_optimization_level_performance);

  options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

  shaderc_shader_kind kind;
  switch (stage) {
    case ShaderStage::VERTEX:
      kind = shaderc_glsl_vertex_shader;
      break;
    case ShaderStage::FRAGMENT:
      kind = shaderc_glsl_fragment_shader;
      break;
    case ShaderStage::COMPUTE:
      kind = shaderc_glsl_compute_shader;
      break;
    default:
      m_lastError = "Unknown stage";
      return false;
  }

  shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
    source,
    kind,
    "shader.glsl",
    entryPoint.c_str(),
    options 
  );

  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    m_lastError = result.GetErrorMessage();
    std::cerr << "Shader compilation failed: " << m_lastError << std::endl;
    return false;
  }

  spirvOut = std::vector<uint32_t>(result.cbegin(), result.cend());
  
  std::cout << "Shader compiled successfully (" << spirvOut.size() << " words)" << std::endl;
  return true;
}
bool ShaderCompiler::compileFromFile(
  const std::string& filePath,
  ShaderStage stage,
  const std::string& entryPoint,
  std::vector<uint32_t>& spirvOut 
) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    m_lastError = "Failed to open file: " + filePath;
    return false;
  }
  
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();

  return compileFromSource(source, stage, entryPoint, spirvOut); 
}

VkShaderModule ShaderCompiler::createShaderModule(
  VkDevice device,
  const std::vector<uint32_t>& spirvCode
) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = spirvCode.size() * sizeof(uint32_t);
  createInfo.pCode = spirvCode.data();

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    m_lastError = "Failed to create shader module";
    return VK_NULL_HANDLE;
  }
  return shaderModule;
}

VkShaderStageFlagBits ShaderCompiler::getVulkanStage(ShaderStage stage) {
  switch (stage) {
    case ShaderStage::VERTEX:
      return VK_SHADER_STAGE_VERTEX_BIT;
    case ShaderStage::FRAGMENT:
      return VK_SHADER_STAGE_FRAGMENT_BIT;
    case ShaderStage::COMPUTE:
      return VK_SHADER_STAGE_COMPUTE_BIT;
  }
}

}

