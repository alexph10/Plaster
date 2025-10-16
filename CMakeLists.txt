cmake_minimum_required(VERSION 3.20)

# Use vcpkg toolchain BEFORE project() call
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(Plaster VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Debug build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -------------------------------------------------------------------
# Find Packages
# -------------------------------------------------------------------
# Windowing + Input
find_package(glfw3 CONFIG REQUIRED)

# Vulkan loader and core API
find_package(volk CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

# Shader compilation and SPIR-V tools
find_package(shaderc CONFIG REQUIRED)
find_package(spirv-tools CONFIG REQUIRED)
find_package(spirv-reflect CONFIG REQUIRED)

# Memory allocator for Vulkan
find_package(vulkan-memory-allocator CONFIG REQUIRED)

# Math library
find_package(glm CONFIG REQUIRED)

# Asset loading
find_package(stb CONFIG REQUIRED)             # stb_image, stb_truetype
find_package(tinyobjloader CONFIG REQUIRED)  # OBJ loader
find_package(gltf-sdk CONFIG REQUIRED)       # glTF 2.0 loader
# Optional: Assimp for other formats
find_package(assimp CONFIG OPTIONAL)

# GUI for debugging
find_package(imgui CONFIG REQUIRED)

# Logging
find_package(spdlog CONFIG REQUIRED)

# Audio (optional)
find_package(OpenAL CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)

# -------------------------------------------------------------------
# Include directories
# -------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/platform
)

# -------------------------------------------------------------------
# Source files
# -------------------------------------------------------------------
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

# -------------------------------------------------------------------
# Link libraries
# -------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    volk::volk
    Vulkan::Vulkan
    shaderc_shared
    spirv-tools-opt     # for SPIR-V optimization
    spirv_reflect
    vma                 # Vulkan Memory Allocator
    glm::glm
    stb::stb_image
    stb::stb_truetype
    tinyobjloader::tinyobjloader
    glTF::glTF
    assimp::assimp      # optional
    imgui::imgui
    spdlog::spdlog
    OpenAL::OpenAL
    SndFile::sndfile
)

# -------------------------------------------------------------------
# Assets
# -------------------------------------------------------------------
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Set VS debugger working directory
set_property(TARGET ${PROJECT_NAME}
    PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

