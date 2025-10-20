#pragma once 
#include "VulkanBuffer.h"
#include <glm/glm.hpp>
#include <ppltasks.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Plaster {
struct PlastibooMaterialData {
  glm::vec4 baseColor;
  float clayRoughness;
  float ditherStrength;
  float warmthBias;
  int paletteIndex;
  int useDithering;
  int useAffineMapping;
  float padding[2];
};

class PlastibooMaterial {
public:
  PlastibooMaterial();
  ~PlastibooMaterial();
  
  bool create(VmaAllocator allocator);

  void destroy(VmaAllocator allocator);
  
  void updateData(VmaAllocator allocator, const PlastibooMaterialData& data);

  VkBuffer getUniformBuffer() const { return m_uniformBuffer.getBuffer(); }

  const PlastibooMaterialData& getData() const { return m_data; }

  void setBaseColor( const glm::vec3& color) { m_data.baseColor = glm::vec4(color, 1.0f); }
  void setClayRoughness(float roughness) { m_data.clayRoughness = roughness; }
  void setDitherStrength(float strength) { m_data.ditherStrength = strength; }
  void setWarmthBias(float bias) { m_data.warmthBias = bias; }
  void setPaletteIndex(int index) { m_data.paletteIndex = index; }
  void setUseDithering(bool use) { m_data.useDithering = use ? 1 : 0; }
  void setUseAffineMapping(bool use) { m_data.useAffineMapping = use ? 1 : 0; }


  static PlastibooMaterialData createMedievalDungeonPreset();
  static PlastibooMaterialData createBloodRitualPreset();
  static PlastibooMaterialData createPlagueVillagePreset();
  static PlastibooMaterialData createAncientForestPreset();

private:
  VulkanBuffer m_uniformBuffer;
  PlastibooMaterialData m_data;
};
}


