#pragma once

#include <vector>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool shouldClose() const;
    void pollEvents() const;
    void waitEvents() const;
    GLFWwindow* getHandle() const { return _window; }
    
    // Vulkan specific methods
    VkSurfaceKHR createSurface(VkInstance instance) const;
    std::vector<const char*> getRequiredExtensions() const;
    
    // Window properties
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    void getFramebufferSize(int& width, int& height) const;
    
    // Events
    bool wasResized() const { return _framebufferResized; }
    void resetResizeFlag() { _framebufferResized = false; }

private:
    GLFWwindow* _window;
    int _width;
    int _height;
    mutable bool _framebufferResized = false;
    
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    
    void initGLFW();
    void createWindow(const char* title);
};
