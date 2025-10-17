#include "Window.h"
#include <stdexcept>
#include <iostream>
#include <limits>
#include <algorithm>

Window::Window(int width, int height, const char* title) : _width(width), _height(height) {
    initGLFW();
    createWindow(title);
}

Window::~Window() {
    if (_window) {
        glfwDestroyWindow(_window);
    }
    glfwTerminate();
}

void Window::initGLFW() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Tell GLFW not to create an OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
}

void Window::createWindow(const char* title) {
    _window = glfwCreateWindow(_width, _height, title, nullptr, nullptr);
    
    if (!_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Set user pointer for callbacks
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(_window);
}

void Window::pollEvents() const {
    glfwPollEvents();
}

void Window::waitEvents() const {
    glfwWaitEvents();
}

VkSurfaceKHR Window::createSurface(VkInstance instance) const {
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    
    VkResult result = glfwCreateWindowSurface(instance, _window, nullptr, &surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface! VkResult: " + std::to_string(result));
    }
    
    return surface;
}

std::vector<const char*> Window::getRequiredExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    return extensions;
}

void Window::getFramebufferSize(int& width, int& height) const {
    glfwGetFramebufferSize(_window, &width, &height);
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->_framebufferResized = true;
}