#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Plaster {

class DescriptorManager {
public:
   DescriptorManager();
   ~DescriptorManager();

      // Initialize descriptor pool and layouts
   bool create(VkDevice device, uint32_t maxFrames);

      // Destroy all descriptor resources
   void destroy(VkDevice device);

      // Create descriptor set layouts
   bool createDescriptorSetLayouts(VkDevice device);

      // Allocate descriptor sets for a frame
   bool allocateDescriptorSets(
      VkDevice device,
      uint32_t frameIndex,
      VkDescriptorSet& cameraSet,
      VkDescriptorSet& objectSet,
      VkDescriptorSet& materialSet
   );

      // Update descriptor sets with buffers
   void updateCameraDescriptor(
      VkDevice device,
      VkDescriptorSet descriptorSet,
      VkBuffer cameraBuffer,
      VkBuffer lightBuffer
   );

   void updateObjectDescriptor(
      VkDevice device,
      VkDescriptorSet descriptorSet,
      VkBuffer objectBuffer
   );

   void updateMaterialDescriptor(
      VkDevice device,
      VkDescriptorSet descriptorSet,
      VkBuffer materialBuffer,
      VkImageView paletteView,
      VkSampler paletteSampler,
      VkImageView blueNoiseView,
      VkSampler blueNoiseSampler,
      VkImageView albedoView,
      VkSampler albedoSampler
   );

   // Getters
   VkDescriptorSetLayout getCameraLayout() const { return m_cameraLayout; }
   VkDescriptorSetLayout getObjectLayout() const { return m_objectLayout; }
   VkDescriptorSetLayout getMaterialLayout() const { return m_materialLayout; }

private:
   VkDescriptorPool m_descriptorPool;

   VkDescriptorSetLayout m_cameraLayout;    // Set 0
   VkDescriptorSetLayout m_objectLayout;    // Set 1
   VkDescriptorSetLayout m_materialLayout;  // Set 2
};

}