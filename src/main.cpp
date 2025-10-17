#include <iostream>
#include <stdexcept>
#include "platform/Window.h"
#include "renderer/VulkanRenderer.h"

int main() {
    try {
        std::cout << "Plaster Engine - Vulkan Window Test" << std::endl;
        
        // Create window
        Window window(1024, 768, "Plaster Engine - Vulkan");
        
        // Create Vulkan renderer
        VulkanRenderer renderer;
        renderer.initialize(window);
        
        std::cout << "Vulkan initialized successfully!" << std::endl;
        std::cout << "Close the window to exit." << std::endl;
        
        // Main loop
        while (!window.shouldClose()) {
            window.pollEvents();
            renderer.drawFrame();
        }
        
        // Cleanup happens automatically when objects are destroyed
        std::cout << "Shutting down gracefully..." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}
