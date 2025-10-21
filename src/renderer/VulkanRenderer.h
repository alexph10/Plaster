#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <vector>
#include <optional>
#include <memory>
#include "../ui/ImGuiManager.h"
#include "DescriptorManager.h"
#include "VulkanBuffer.h"
#include "UniformBuffers.h"

class Window;

namespace Plaster {
    class Scene;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void initialize(const Window& window);
    void cleanup();
    void drawFrame();
    void renderScene(Plaster::Scene& scene);

    bool isInitialized() const { return _initialized; }

    VkDevice getDevice() const { return _device; }
    VkPhysicalDevice getPhysicalDevice() const { return _physicalDevice; }
    VkCommandPool getCommandPool() const { return _commandPool; }
    VkQueue getGraphicsQueue() const { return _graphicsQueue; }
    VmaAllocator getAllocator() const { return _allocator; }

private:
    // Core Vulkan objects
    VkInstance _instance = VK_NULL_HANDLE;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkQueue _graphicsQueue = VK_NULL_HANDLE;
    VkQueue _presentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

    // Swap chain
    VkSwapchainKHR _swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> _swapChainImages;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;
    std::vector<VkImageView> _swapChainImageViews;

    // Render pass and pipeline
    VkRenderPass _renderPass = VK_NULL_HANDLE;
    VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
    VkPipeline _graphicsPipeline = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> _swapChainFramebuffers;

    // Command buffers
    VkCommandPool _commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> _commandBuffers;

    // Synchronization
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    std::vector<VkFence> _imagesInFlight; // Track fences for each swapchain image

    // Frame tracking
    uint32_t _currentFrame = 0;
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    bool _initialized = false;
    const Window* _window = nullptr;

    // ImGui Manager
    ImGuiManager _imguiManager;

    // VMA Allocator
    VmaAllocator _allocator = VK_NULL_HANDLE;

    // Plaster rendering components
    Plaster::DescriptorManager _descriptorManager;

    // Uniform buffers (per frame)
    std::vector<Plaster::VulkanBuffer> _cameraBuffers;
    std::vector<Plaster::VulkanBuffer> _lightBuffers;
    std::vector<Plaster::VulkanBuffer> _objectBuffers;

    // Descriptor sets (per frame)
    std::vector<VkDescriptorSet> _cameraDescriptorSets;
    std::vector<VkDescriptorSet> _objectDescriptorSets;
    std::vector<VkDescriptorSet> _materialDescriptorSets;

    // Scene being rendered
    Plaster::Scene* _currentScene = nullptr;

    // Validation layers
#ifdef NDEBUG
    static const bool enableValidationLayers = false;
#else
    static const bool enableValidationLayers = true;
#endif

    const std::vector<const char*> _validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> _deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Initialization methods
    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createAllocator();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorResources();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    // Helper methods
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    // Command buffer recording
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    // Cleanup helpers
    void cleanupSwapChain();
    void recreateSwapChain();
};