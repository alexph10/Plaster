  #pragma once

  #include <vulkan/vulkan.h>
  #include <vk_mem_alloc.h>
  #include <string>

  namespace Plaster {

  class Texture {
  public:
      Texture();
      ~Texture();

      // Create texture from raw data
      bool createFromData(
          VmaAllocator allocator,
          VkDevice device,
          VkCommandPool commandPool,
          VkQueue graphicsQueue,
          const void* pixels,
          uint32_t width,
          uint32_t height,
          VkFormat format,
          VkFilter filter = VK_FILTER_NEAREST
      );

      // Create placeholder 1x1 white texture
      bool createPlaceholder(
          VmaAllocator allocator,
          VkDevice device,
          VkCommandPool commandPool,
          VkQueue graphicsQueue
      );

      // Destroy texture
      void destroy(VmaAllocator allocator, VkDevice device);

      // Getters
      VkImage getImage() const { return m_image; }
      VkImageView getImageView() const { return m_imageView; }
      VkSampler getSampler() const { return m_sampler; }
      uint32_t getWidth() const { return m_width; }
      uint32_t getHeight() const { return m_height; }

  private:
      VkImage m_image;
      VmaAllocation m_allocation;
      VkImageView m_imageView;
      VkSampler m_sampler;
      uint32_t m_width;
      uint32_t m_height;

      void transitionImageLayout(
          VkDevice device,
          VkCommandPool commandPool,
          VkQueue graphicsQueue,
          VkImage image,
          VkFormat format,
          VkImageLayout oldLayout,
          VkImageLayout newLayout
      );

      void copyBufferToImage(
          VkDevice device,
          VkCommandPool commandPool,
          VkQueue graphicsQueue,
          VkBuffer buffer,
          VkImage image,
          uint32_t width,
          uint32_t height
      );
  };

  }