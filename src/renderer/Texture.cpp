  #include "Texture.h"
  #include "VulkanBuffer.h"
  #include <iostream>

  namespace Plaster {

  Texture::Texture()
      : m_image(VK_NULL_HANDLE)
      , m_allocation(nullptr)
      , m_imageView(VK_NULL_HANDLE)
      , m_sampler(VK_NULL_HANDLE)
      , m_width(0)
      , m_height(0)
  {
  }

  Texture::~Texture() {
  }

  bool Texture::createFromData(
      VmaAllocator allocator,
      VkDevice device,
      VkCommandPool commandPool,
      VkQueue graphicsQueue,
      const void* pixels,
      uint32_t width,
      uint32_t height,
      VkFormat format,
      VkFilter filter
  ) {
      m_width = width;
      m_height = height;

      VkDeviceSize imageSize = width * height * 4; // Assuming 4 bytes per pixel (RGBA)

      // Create staging buffer
      VulkanBuffer stagingBuffer;
      if (!stagingBuffer.create(
          allocator,
          imageSize,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
          VMA_MEMORY_USAGE_CPU_ONLY,
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
      )) {
          return false;
      }

      stagingBuffer.copyData(allocator, pixels, imageSize);

      // Create image
      VkImageCreateInfo imageInfo{};
      imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageInfo.imageType = VK_IMAGE_TYPE_2D;
      imageInfo.extent.width = width;
      imageInfo.extent.height = height;
      imageInfo.extent.depth = 1;
      imageInfo.mipLevels = 1;
      imageInfo.arrayLayers = 1;
      imageInfo.format = format;
      imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
      imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

      VmaAllocationCreateInfo allocInfo{};
      allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

      if (vmaCreateImage(allocator, &imageInfo, &allocInfo, &m_image, &m_allocation, nullptr) != VK_SUCCESS) {
          std::cerr << "Failed to create image!" << std::endl;
          stagingBuffer.destroy(allocator);
          return false;
      }

      // Transition layout and copy
      transitionImageLayout(device, commandPool, graphicsQueue, m_image, format,
                           VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

      copyBufferToImage(device, commandPool, graphicsQueue, stagingBuffer.getBuffer(),
                       m_image, width, height);

      transitionImageLayout(device, commandPool, graphicsQueue, m_image, format,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

      stagingBuffer.destroy(allocator);

      // Create image view
      VkImageViewCreateInfo viewInfo{};
      viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      viewInfo.image = m_image;
      viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      viewInfo.format = format;
      viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      viewInfo.subresourceRange.baseMipLevel = 0;
      viewInfo.subresourceRange.levelCount = 1;
      viewInfo.subresourceRange.baseArrayLayer = 0;
      viewInfo.subresourceRange.layerCount = 1;

      if (vkCreateImageView(device, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
          std::cerr << "Failed to create image view!" << std::endl;
          return false;
      }

      // Create sampler
      VkSamplerCreateInfo samplerInfo{};
      samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerInfo.magFilter = filter;
      samplerInfo.minFilter = filter;
      samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.anisotropyEnable = VK_FALSE;
      samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
      samplerInfo.unnormalizedCoordinates = VK_FALSE;
      samplerInfo.compareEnable = VK_FALSE;
      samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

      if (vkCreateSampler(device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
          std::cerr << "Failed to create sampler!" << std::endl;
          return false;
      }

      std::cout << "Texture created: " << width << "x" << height << std::endl;
      return true;
  }

  bool Texture::createPlaceholder(
      VmaAllocator allocator,
      VkDevice device,
      VkCommandPool commandPool,
      VkQueue graphicsQueue
  ) {
      uint32_t white = 0xFFFFFFFF; // RGBA white
      return createFromData(allocator, device, commandPool, graphicsQueue,
                           &white, 1, 1, VK_FORMAT_R8G8B8A8_UNORM, VK_FILTER_NEAREST);
  }

  void Texture::destroy(VmaAllocator allocator, VkDevice device) {
      if (m_sampler != VK_NULL_HANDLE) {
          vkDestroySampler(device, m_sampler, nullptr);
          m_sampler = VK_NULL_HANDLE;
      }
      if (m_imageView != VK_NULL_HANDLE) {
          vkDestroyImageView(device, m_imageView, nullptr);
          m_imageView = VK_NULL_HANDLE;
      }
      if (m_image != VK_NULL_HANDLE) {
          vmaDestroyImage(allocator, m_image, m_allocation);
          m_image = VK_NULL_HANDLE;
          m_allocation = nullptr;
      }
  }

  void Texture::transitionImageLayout(
      VkDevice device,
      VkCommandPool commandPool,
      VkQueue graphicsQueue,
      VkImage image,
      VkFormat format,
      VkImageLayout oldLayout,
      VkImageLayout newLayout
  ) {
      VkCommandBufferAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      allocInfo.commandPool = commandPool;
      allocInfo.commandBufferCount = 1;

      VkCommandBuffer commandBuffer;
      vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

      vkBeginCommandBuffer(commandBuffer, &beginInfo);

      VkImageMemoryBarrier barrier{};
      barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.oldLayout = oldLayout;
      barrier.newLayout = newLayout;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = image;
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;

      VkPipelineStageFlags sourceStage;
      VkPipelineStageFlags destinationStage;

      if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
          barrier.srcAccessMask = 0;
          barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
          sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
          destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
          barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
          barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
          sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
          destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      } else {
          throw std::runtime_error("Unsupported layout transition!");
      }

      vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

      vkEndCommandBuffer(commandBuffer);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &commandBuffer;

      vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
      vkQueueWaitIdle(graphicsQueue);

      vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
  }

  void Texture::copyBufferToImage(
      VkDevice device,
      VkCommandPool commandPool,
      VkQueue graphicsQueue,
      VkBuffer buffer,
      VkImage image,
      uint32_t width,
      uint32_t height
  ) {
      VkCommandBufferAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      allocInfo.commandPool = commandPool;
      allocInfo.commandBufferCount = 1;

      VkCommandBuffer commandBuffer;
      vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

      vkBeginCommandBuffer(commandBuffer, &beginInfo);

      VkBufferImageCopy region{};
      region.bufferOffset = 0;
      region.bufferRowLength = 0;
      region.bufferImageHeight = 0;
      region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      region.imageSubresource.mipLevel = 0;
      region.imageSubresource.baseArrayLayer = 0;
      region.imageSubresource.layerCount = 1;
      region.imageOffset = {0, 0, 0};
      region.imageExtent = {width, height, 1};

      vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

      vkEndCommandBuffer(commandBuffer);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &commandBuffer;

      vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
      vkQueueWaitIdle(graphicsQueue);

      vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
  }

  