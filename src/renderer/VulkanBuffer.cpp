#include "VulkanBuffer.h"
#include <cstring>
#include <iostream>
#include <ppltasks.h>

namespace Plaster {
VulkanBuffer::VulkanBuffer()
  : m_buffer(VK_NULL_HANDLE)
  , m_allocation(nullptr)
  , m_size(0)
{
}

VulkanBuffer::~VulkanBuffer() {
  if (m_buffer != VK_NULL_HANDLE) {
    std::cerr << "Warning: VulkanBuffer destroyed without explicit cleanup" << std::endl;
  }
}

bool VulkanBuffer::create (
  VmaAllocator allocator,
  VkDeviceSize size,
  VkBufferUsageFlags usage,
  VmaMemoryUsage memoryUsage,
  VmaAllocationCreateFlags flags 
) {
  m_size = size;

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = memoryUsage;
  allocInfo.flags = flags;

  if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr) != VK_SUCCESS) {
    std::cerr << "Failed to create Vulkan Buffer" << std::endl;
    return false;
  }
  return true;
}

void VulkanBuffer::destroy(VmaAllocator allocator) {
  if (m_buffer != VK_NULL_HANDLE) {
    vmaDestroyBuffer(allocator, m_buffer, m_allocation);
    m_buffer = VK_NULL_HANDLE;
    m_allocation = nullptr;
    m_size = 0;
  }
}

void* VulkanBuffer::map(VmaAllocator allocator) {
  void* data;
  vmaMapMemory(allocator, m_allocation, &data);
  return data;
}

void VulkanBuffer::unmap(VmaAllocator allocator) {
  VmaUnmapMemory(allocator, m_allocation);
}

void VulkanBuffer::copyData(VmaAllocator allocator, const void* data, VkDeviceSize size) {
  void* mapped = map(allocator);
  memcpy(mapped, data, size);
  unmap(allocator);
}

}
