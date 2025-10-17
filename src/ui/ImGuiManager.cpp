#include "ImGuiManager.h"
#include "ImGuiTheme.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <array>

ImGuiManager::ImGuiManager()
    : _instance(VK_NULL_HANDLE)
    , _device(VK_NULL_HANDLE)
    , _physicalDevice(VK_NULL_HANDLE)
    , _graphicsQueue(VK_NULL_HANDLE)
    , _queueFamily(0)
    , _renderPass(VK_NULL_HANDLE)
    , _descriptorPool(VK_NULL_HANDLE)
    , _window(nullptr)
    , _minImageCount(2)
    , _imageCount(2)
    , _initialized(false)
{    
}

ImGuiManager::~ImGuiManager() {
    if (_initialized) {
        shutdown();
    }
}

void ImGuiManager::init(
    GLFWwindow* window,
    VkInstance instance,
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    uint32_t queueFamily,
    VkQueue queue,
    VkRenderPass renderPass,
    uint32_t imageCount
) {
    _window = window;
    _instance = instance;
    _physicalDevice = physicalDevice; 
    _device = device;
    _queueFamily = queueFamily;
    _graphicsQueue = queue;
    _renderPass = renderPass;
    _imageCount = imageCount;
    _minImageCount = imageCount;

    createDescriptorPool();

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    
    // Enable alpha blending for translucent acrylic windows
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
    
    // Apply custom theme
    ImGuiTheme::ApplyOrangeAcrylicTheme();

    // Initialize ImGui for GLFW and Vulkan
    ImGui_ImplGlfw_InitForVulkan(_window, true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = _instance;
    init_info.PhysicalDevice = physicalDevice;
    init_info.Device = _device;
    init_info.QueueFamily = _queueFamily;
    init_info.Queue = _graphicsQueue;
    init_info.DescriptorPool = _descriptorPool;
    init_info.MinImageCount = _minImageCount;
    init_info.ImageCount = _imageCount;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = nullptr;
    init_info.RenderPass = _renderPass;

    ImGui_ImplVulkan_Init(&init_info);

    uploadFonts();

    _initialized = true;
}

void ImGuiManager::newFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

}

void ImGuiManager::render(VkCommandBuffer commandBuffer) {
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);

    const ImGuiIO& io = ImGui::GetIO();
#ifdef ImGuiConfigFlags_ViewportsEnable
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
#endif
}

void ImGuiManager::shutdown() {
    if (!_initialized) return;

    vkDeviceWaitIdle(_device);
    
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    if (_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
        _descriptorPool = VK_NULL_HANDLE;
    }
    _initialized = false;
}

void ImGuiManager::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 11> pool_sizes = {{
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    }};
    VkDescriptorPoolCreateInfo pool_info = {};
    
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * pool_sizes.size();
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();

    if (vkCreateDescriptorPool(_device, &pool_info, nullptr, &_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool.");
    }
}

void ImGuiManager::uploadFonts() {
    // Modern ImGui API: CreateFontsTexture() no longer takes a command buffer
    // It now handles command buffer creation internally
    ImGui_ImplVulkan_CreateFontsTexture();
    
    // Clean up after upload - modern API function name
    ImGui_ImplVulkan_DestroyFontsTexture();
}


