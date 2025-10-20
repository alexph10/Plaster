#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace Plaster {
enum class ShaderStage {
  VERTEX,
  FRAGMENT,
  COMPUTE 
};

struct ShaderModule {
  VkShaderModule module;
  ShaderStage stage;
  std::vector<uint32> spirvCode;
  std::string entryPoint;
};

class ShaderCompiler {
public:
  ShaderCompiler();
  ~ShaderCompiler();

  bool compileFromSource() {
    const std::string& source,
    ShaderStage stage, 
    const std::string& entryPoint,
    std::vector<uint32_t>& spirvOut
  };
  
  bool compileFromFile() {
      const std::string& filePath,
      ShaderStage stage,
      const std::string& entryPoint,
      std::vector<uint32_t>& spirvOut 
  };

  VkShaderModule createShaderModule(
      VkDevice device,
      const std::vector<uin32_t>& spirvCode 
  );
  
  const std::string& getLastError() const { return m_lastError; }

private:
    std::string m_lastError;
    
    VkShaderStageFlagBits getVulkanStage(ShaderStage stage);
};
}

