#include "Mesh.h"
#include <winuser.h>
#include <iostream

namespace Plaster {
Mesh::Mesh()
  : m_vertexCount(0)
  , m_indexCount(0)
{
}

Mesh::~Mesh(){
}

bool Mesh::create(
  VmaAllocator allocator,
  VkDevice device, 
  VkCommandPool commandPool,
  VkQueue graphicsQueue,
  const std::vector<PlastibooVertex>& vertices,
  const std::vector<uint32_t>& indices
) {
  m_vertexCount = vertices.size();
  m_indexCount = indices.size();

  VkDeviceSize vertexBufferSize = sizeof(PlastibooVertex) * vertices.size();

  if (!createBufferWithStaging(
    allocator, device, commandPool, graphicsQueue,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    vertices.data(), vertexBufferSize, m_vertexBuffer
  )) {
    std::cerr << "Failed to create vertex buffer" << std::endl;
    return false;
  }

  VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();
  if (!createBufferWithStaging(
    allocator, device, commandPool, graphicsQueue,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    indices.data(), indexBufferSize, m_indexBuffer 
  )) {
    std::cerr << "Failed to create index buffer" << std::endl;
    return false;
  }

  std::cout << "Mesh created: " << m_vertexCount << " vertices, "
    << m_indexCount << "indices" << std::endl;

  return true;
}

void Mesh::destroy(VmaAllocator allocator) {
  m_vertexBuffer.destroy(allocator);
  m_indexBuffer.destroy(allocator);
  m_vertexCount = 0;
  m_indexCount = 0;
}

void Mesh::bind(VkCommandBuffer commandBuffer) {
  VkBuffer vertexBuffer[] = { m_vertexBuffer.getBuffer() };
  VkDeviceSize offsets[] = { 0 };

  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
  
  vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::draw(VkCommandBuffer commandBuffer) {
  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indexCount), 1, 0, 0, 0);
}

bool Mesh::createBufferWithStaging(
  VmaAllocator allocator,
  VkDevice device,
  VkCommandPool commandPool,
  VkQueue graphicsQueue,
  VkBufferUsageFlags usage,
  const void* data,
  VkDeviceSize size,
  VulkanBuffer& buffer 
) {
  VulkanBuffer stagingBuffer;
  if (!stagingBuffer.create(
    allocator,
    size,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VMA_MEMORY_USAGE_CPU_ONLY,
    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
  )) {
    return false;
  }

  stagingBuffer.copyData(allocator, data, size);
  

  if (!buffer.create(
    allocator,
    size, 
    usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VMA_MEMORY_USAGE_GPU_ONLY,
    0
  )) {
    stagingBuffer.destroy(allocator);
    return false;
  }

  copyBuffer(device, commandPool, graphicsQueue, stagingBuffer.getBuffer(), buffer.getBuffer(), size);

  stagingBuffer.destroy(allocator);

  return true;
}

void Mesh::copyBuffer(
  VkDevice device,
  VkCommandPool commandPool,
  VkQueue graphicsQueue,
  VkBuffer srcBuffer,
  VkBuffer dstBuffer,
  VkDeviceSize size 
) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount= 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  
  vkBeginCommandBuffer(commandBuffer, &beginInfo);
  
  VkBufferCopy copyRegion{};
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size = size;

  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue);
  
  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

}

}
