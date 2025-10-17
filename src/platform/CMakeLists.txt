add_library(Platform
    Window.cpp 
)

target_link_libraries(Platform PUBLIC
    glfw, 
    Vulkan::Vulkan
)

target_include_directories(Platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
