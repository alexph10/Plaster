#pragma once

#include <vulkan/vulkan.h>
#include <vector>


struct GLFWwindow;

class ImGuiManager {
    public:
        ImGuiManager();
        ~ImGuiManager();
        
        ImGuiManager(const ImGuiManager&) = delete;
        ImGuiManager& operator =(const ImGuiManager&) = delete;

        void init(
            GLFWwindow* window,
            VkInstance instance,
            
            VkPhysicalDevice physicalDevice,
            VkDevice device,
            uint32_t queueFamily,
            VkQueue queue,
            VkRenderPass renderPass, 
            uint32_t imageCount 
        );

        void newFrame();
        
        void render(VkCommandBuffer commandBuffer);
        
        void shutdown();

    private:
        VkInstance _instance;
        VkDevice _device;
        VkPhysicalDevice _physicalDevice;
        VkQueue _graphicsQueue;
        uint32_t _queueFamily;
        VkRenderPass _renderPass;
        VkDescriptorPool _descriptorPool;
        
        GLFWwindow* _window;
        
        uint32_t _minImageCount;
        uint32_t _imageCount;
        
        bool _initialized;
        
        void createDescriptorPool();
        void uploadFonts();

};