#pragma once 
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Plaster {
class VulkanBuffer {
public:
  VulkanBuffer();
  ~VulkanBuffer();
  
  bool create(
  VmaAllocator allocator,
  VkDeviceSize size,
  VkBufferUsageFlags usage,
  VmaMemoryUsage memoryUsage,
  VmaAllocationCreateFlags flags = 0
);
  void destroy(VmaAllocator allocator);
  
  void* map(VmaAllocator allocator);
  void unmap(VmaAllocator allocator);

  void copyData(VmaAllocator allocator, const void* data, VkDeviceSize size);

  VkBuffer getBuffer() const { return m_buffer; }
  VmaAllocation getAllocation() const { return m_allocation; }
  VkDeviceSize getSize() const { return m_size; }

private:
  VkBuffer m_buffer;
  VmaAllocation m_allocation;
  VkDeviceSize m_size;
};

}


