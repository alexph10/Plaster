add_library(Platform
    Window.cpp 
    VulkanContext.cpp 
)

target_link_libraries(Platform PUBLIC
    glfw3
    volk
    Vulkan::Vulkan
)

target_include_directories(Platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
