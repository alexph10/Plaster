#pragma once
#include "VulkanBuffer.h"
#include <glm/glm.hpp>
#include <ppltasks.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>


namespace Plaster {
struct PlastibooVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;
  glm::vec3 color;
  
  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(PlastibooVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
  }

  static std::vector<VkVertexInputBindingDescription> getAttributeDescriptions() {
    std::vector<VkVertexInputBindingDescription> attributeDescriptions(4);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(PlastibooVertex, position);
    
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(PlastibooVertex, normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(PlastibooVertex, texCoord);

    attributeDescriptions[3].binding = 0;
    attributeDescription[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(PlastibooVertex, color);
    
    return attributeDescriptions;
  }
};

class Mesh {
public:
  Mesh();
  ~Mesh();

  bool create(
    VmaAllocator allocator,
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue graphicsQueue,
    const std::vector<PlastibooVertex>& vertices,
    const std::vector<uint32_t>& indices
  );

  void destroy(VmaAllocator allocator);

  void bind(VkCommandBuffer commandBuffer);
  
  void draw(VkCommandBuffer commandBuffer);

  size_t getVertexCount() const { return m_vertexCount; }
  size_t getIndexCount() const { return m_indexCount; }

private:
  VulkanBuffer m_vertexBuffer;
  VulkanBuffer m_indexBuffer;
  size_t m_vertexCount;
  size_t m_indexCount;

  bool createBufferWithStaging(
    VmaAllocator allocator,
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue graphicsQueue,
    VkBufferUsageFlags usage,
    const void* data,
    VkDeviceSize size,
    VulkanBuffer& buffer 
  );

  void copyBuffer(
    VkDevice device,
    VkCommandPool commandPool,
    VkQueue graphicsQueue,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size 
  );
};

}
