// This file contains the Plastiboo-specific rendering integration for VulkanRenderer
// Add these implementations to VulkanRenderer.cpp

#include "VulkanRenderer.h"
#include "ShaderCompiler.h"
#include "Mesh.h"
#include "../scene/Scene.h"
#include <stdexcept>
#include <iostream>

// Add to VulkanRenderer::initialize() after createLogicalDevice():
void VulkanRenderer::createAllocator() {
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
    allocatorInfo.physicalDevice = _physicalDevice;
    allocatorInfo.device = _device;
    allocatorInfo.instance = _instance;

    if (vmaCreateAllocator(&allocatorInfo, &_allocator) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create VMA allocator!");
    }

    std::cout << "VMA Allocator created" << std::endl;
}

// Add to VulkanRenderer::initialize() after createRenderPass():
void VulkanRenderer::createDescriptorResources() {
    // Create descriptor manager
    if (!_descriptorManager.create(_device, MAX_FRAMES_IN_FLIGHT)) {
        throw std::runtime_error("Failed to create descriptor manager!");
    }

    if (!_descriptorManager.createDescriptorSetLayouts(_device)) {
        throw std::runtime_error("Failed to create descriptor set layouts!");
    }

    // Create uniform buffers for each frame
    _cameraBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    _lightBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    _objectBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (!_cameraBuffers[i].create(_allocator, sizeof(Plaster::CameraUBO),
                                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                      VMA_MEMORY_USAGE_CPU_TO_GPU,
                                      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)) {
            throw std::runtime_error("Failed to create camera uniform buffer!");
        }

        if (!_lightBuffers[i].create(_allocator, sizeof(Plaster::LightUBO),
                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                     VMA_MEMORY_USAGE_CPU_TO_GPU,
                                     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)) {
            throw std::runtime_error("Failed to create light uniform buffer!");
        }

        if (!_objectBuffers[i].create(_allocator, sizeof(Plaster::ObjectUBO),
                                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                      VMA_MEMORY_USAGE_CPU_TO_GPU,
                                      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)) {
            throw std::runtime_error("Failed to create object uniform buffer!");
        }
    }

    // Allocate descriptor sets
    _cameraDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    _objectDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    _materialDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (!_descriptorManager.allocateDescriptorSets(_device, i,
                                                       _cameraDescriptorSets[i],
                                                       _objectDescriptorSets[i],
                                                       _materialDescriptorSets[i])) {
            throw std::runtime_error("Failed to allocate descriptor sets!");
        }

        // Update camera descriptor set
        _descriptorManager.updateCameraDescriptor(_device, _cameraDescriptorSets[i],
                                                  _cameraBuffers[i].getBuffer(),
                                                  _lightBuffers[i].getBuffer());

        // Update object descriptor set
        _descriptorManager.updateObjectDescriptor(_device, _objectDescriptorSets[i],
                                                  _objectBuffers[i].getBuffer());
    }

    std::cout << "Descriptor resources created" << std::endl;
}

// REPLACE the existing createGraphicsPipeline() implementation:
void VulkanRenderer::createGraphicsPipeline() {
    using namespace Plaster;

    // Compile shaders
    ShaderCompiler compiler;
    std::vector<uint32_t> vertSpirv, fragSpirv;

    if (!compiler.compileFromFile("src/shaders/plastiboo.vert", ShaderCompiler::ShaderType::Vertex, vertSpirv)) {
        throw std::runtime_error("Failed to compile vertex shader!");
    }

    if (!compiler.compileFromFile("src/shaders/plastiboo.frag", ShaderCompiler::ShaderType::Fragment, fragSpirv)) {
        throw std::runtime_error("Failed to compile fragment shader!");
    }

    // Create shader modules
    VkShaderModule vertShaderModule = compiler.createShaderModule(_device, vertSpirv);
    VkShaderModule fragShaderModule = compiler.createShaderModule(_device, fragSpirv);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // Vertex input using PlastibooVertex
    auto bindingDescription = PlastibooVertex::getBindingDescription();
    auto attributeDescriptions = PlastibooVertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)_swapChainExtent.width;
    viewport.height = (float)_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = _swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // Pipeline layout with descriptor sets
    std::vector<VkDescriptorSetLayout> setLayouts = {
        _descriptorManager.getCameraLayout(),
        _descriptorManager.getObjectLayout(),
        _descriptorManager.getMaterialLayout()
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
    pipelineLayoutInfo.pSetLayouts = setLayouts.data();

    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass;
    pipelineInfo.subpass = 0;

    if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(_device, vertShaderModule, nullptr);

    std::cout << "Plastiboo graphics pipeline created" << std::endl;
}

// Add this new method for rendering a scene:
void VulkanRenderer::renderScene(Plaster::Scene& scene) {
    _currentScene = &scene;

    // Update camera aspect ratio
    float aspect = (float)_swapChainExtent.width / (float)_swapChainExtent.height;
    scene.getCamera().setAspectRatio(aspect);

    // Update camera uniform buffer
    Plaster::CameraUBO cameraData{};
    cameraData.view = scene.getCamera().getViewMatrix();
    cameraData.projection = scene.getCamera().getProjectionMatrix();
    cameraData.cameraPos = scene.getCamera().getPosition();

    _cameraBuffers[_currentFrame].copyData(_allocator, &cameraData, sizeof(Plaster::CameraUBO));

    // Update light uniform buffer
    _lightBuffers[_currentFrame].copyData(_allocator, &scene.getLights(), sizeof(Plaster::LightUBO));
}

// ADD TO recordCommandBuffer() - replace the comment "For now, just clear the screen":
// After vkCmdBeginRenderPass and before _imguiManager.render():
/*
    if (_currentScene && _graphicsPipeline != VK_NULL_HANDLE) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

        // Bind camera descriptor set (set 0)
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout,
                               0, 1, &_cameraDescriptorSets[_currentFrame], 0, nullptr);

        // Render each object
        for (const auto& obj : _currentScene->getObjects()) {
            // Update object uniform buffer
            Plaster::ObjectUBO objectData{};
            objectData.model = obj.getModelMatrix();
            objectData.normalMatrix = obj.getNormalMatrix();
            _objectBuffers[_currentFrame].copyData(_allocator, &objectData, sizeof(Plaster::ObjectUBO));

            // Bind object descriptor set (set 1)
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout,
                                   1, 1, &_objectDescriptorSets[_currentFrame], 0, nullptr);

            // For now, skip material descriptor set (set 2) - we'll add texture support later

            // Bind mesh and draw
            obj.mesh->bind(commandBuffer);
            obj.mesh->draw(commandBuffer);
        }
    }
*/

// ADD TO cleanup() - before cleanupSwapChain():
/*
    // Cleanup Plastiboo resources
    _descriptorManager.destroy(_device);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _cameraBuffers[i].destroy(_allocator);
        _lightBuffers[i].destroy(_allocator);
        _objectBuffers[i].destroy(_allocator);
    }

    if (_allocator != VK_NULL_HANDLE) {
        vmaDestroyAllocator(_allocator);
        _allocator = VK_NULL_HANDLE;
    }
*/
