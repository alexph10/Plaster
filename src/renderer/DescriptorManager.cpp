#include "DescriptorManager.h"
#include <iostream>
#include <array>

namespace Plaster {

DescriptorManager::DescriptorManager()
    : m_descriptorPool(VK_NULL_HANDLE)
    , m_cameraLayout(VK_NULL_HANDLE)
    , m_objectLayout(VK_NULL_HANDLE)
    , m_materialLayout(VK_NULL_HANDLE)
{
}

DescriptorManager::~DescriptorManager() {
}

bool DescriptorManager::create(VkDevice device, uint32_t maxFrames) {
    // Create descriptor pool
    std::array<VkDescriptorPoolSize, 3> poolSizes{};

    // Uniform buffers (camera, light, object, material)
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = maxFrames * 10; // Generous allocation

    // Samplers (palette, blue noise, albedo)
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = maxFrames * 10;

    // Storage buffers (if needed later)
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[2].descriptorCount = maxFrames * 4;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxFrames * 20; // Max descriptor sets

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        std::cerr << "Failed to create descriptor pool!" << std::endl;
        return false;
    }

    // Create layouts
    if (!createDescriptorSetLayouts(device)) {
        return false;
    }

    std::cout << "DescriptorManager created" << std::endl;
    return true;
}

void DescriptorManager::destroy(VkDevice device) {
    if (m_cameraLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, m_cameraLayout, nullptr);
    }
    if (m_objectLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, m_objectLayout, nullptr);
    }
    if (m_materialLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, m_materialLayout, nullptr);
    }
    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
    }
}

bool DescriptorManager::createDescriptorSetLayouts(VkDevice device) {
    // Set 0: Camera + Lights
    {
        std::array<VkDescriptorSetLayoutBinding, 2> bindings{};

        // Binding 0: Camera UBO
        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        // Binding 1: Light UBO
        bindings[1].binding = 1;
        bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[1].descriptorCount = 1;
        bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_cameraLayout) != VK_SUCCESS) {
            std::cerr << "Failed to create camera descriptor set layout!" << std::endl;
            return false;
        }
    }

    // Set 1: Object transform
    {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &binding;

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_objectLayout) != VK_SUCCESS) {
            std::cerr << "Failed to create object descriptor set layout!" << std::endl;
            return false;
        }
    }

    // Set 2: Material + Textures
    {
        std::array<VkDescriptorSetLayoutBinding, 4> bindings{};

        // Binding 0: Material UBO
        bindings[0].binding = 0;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        // Binding 1: Palette texture
        bindings[1].binding = 1;
        bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[1].descriptorCount = 1;
        bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        // Binding 2: Blue noise texture
        bindings[2].binding = 2;
        bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[2].descriptorCount = 1;
        bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        // Binding 3: Albedo texture
        bindings[3].binding = 3;
        bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[3].descriptorCount = 1;
        bindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_materialLayout) != VK_SUCCESS) {
            std::cerr << "Failed to create material descriptor set layout!" << std::endl;
            return false;
        }
    }

    return true;
}

bool DescriptorManager::allocateDescriptorSets(
    VkDevice device,
    uint32_t frameIndex,
    VkDescriptorSet& cameraSet,
    VkDescriptorSet& objectSet,
    VkDescriptorSet& materialSet
) {
    std::array<VkDescriptorSetLayout, 3> layouts = {
        m_cameraLayout,
        m_objectLayout,
        m_materialLayout
    };

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();

    std::array<VkDescriptorSet, 3> sets;
    if (vkAllocateDescriptorSets(device, &allocInfo, sets.data()) != VK_SUCCESS) {
        std::cerr << "Failed to allocate descriptor sets!" << std::endl;
        return false;
    }

    cameraSet = sets[0];
    objectSet = sets[1];
    materialSet = sets[2];

    return true;
}

void DescriptorManager::updateCameraDescriptor(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    VkBuffer cameraBuffer,
    VkBuffer lightBuffer
) {
    std::array<VkWriteDescriptorSet, 2> writes{};

    VkDescriptorBufferInfo cameraInfo{};
    cameraInfo.buffer = cameraBuffer;
    cameraInfo.offset = 0;
    cameraInfo.range = VK_WHOLE_SIZE;

    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = descriptorSet;
    writes[0].dstBinding = 0;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &cameraInfo;

    VkDescriptorBufferInfo lightInfo{};
    lightInfo.buffer = lightBuffer;
    lightInfo.offset = 0;
    lightInfo.range = VK_WHOLE_SIZE;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = descriptorSet;
    writes[1].dstBinding = 1;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[1].descriptorCount = 1;
    writes[1].pBufferInfo = &lightInfo;

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

void DescriptorManager::updateObjectDescriptor(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    VkBuffer objectBuffer
) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = objectBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descriptorSet;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void DescriptorManager::updateMaterialDescriptor(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    VkBuffer materialBuffer,
    VkImageView paletteView,
    VkSampler paletteSampler,
    VkImageView blueNoiseView,
    VkSampler blueNoiseSampler,
    VkImageView albedoView,
    VkSampler albedoSampler
) {
    std::array<VkWriteDescriptorSet, 4> writes{};

    // Material buffer
    VkDescriptorBufferInfo materialInfo{};
    materialInfo.buffer = materialBuffer;
    materialInfo.offset = 0;
    materialInfo.range = VK_WHOLE_SIZE;

    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = descriptorSet;
    writes[0].dstBinding = 0;
    writes[0].dstArrayElement = 0;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].descriptorCount = 1;
    writes[0].pBufferInfo = &materialInfo;

    // Palette texture
    VkDescriptorImageInfo paletteImageInfo{};
    paletteImageInfo.imageView = paletteView;
    paletteImageInfo.sampler = paletteSampler;
    paletteImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = descriptorSet;
    writes[1].dstBinding = 1;
    writes[1].dstArrayElement = 0;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].descriptorCount = 1;
    writes[1].pImageInfo = &paletteImageInfo;

    // Blue noise texture
    VkDescriptorImageInfo blueNoiseImageInfo{};
    blueNoiseImageInfo.imageView = blueNoiseView;
    blueNoiseImageInfo.sampler = blueNoiseSampler;
    blueNoiseImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[2].dstSet = descriptorSet;
    writes[2].dstBinding = 2;
    writes[2].dstArrayElement = 0;
    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[2].descriptorCount = 1;
    writes[2].pImageInfo = &blueNoiseImageInfo;

    // Albedo texture
    VkDescriptorImageInfo albedoImageInfo{};
    albedoImageInfo.imageView = albedoView;
    albedoImageInfo.sampler = albedoSampler;
    albedoImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    writes[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[3].dstSet = descriptorSet;
    writes[3].dstBinding = 3;
    writes[3].dstArrayElement = 0;
    writes[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[3].descriptorCount = 1;
    writes[3].pImageInfo = &albedoImageInfo;

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

}