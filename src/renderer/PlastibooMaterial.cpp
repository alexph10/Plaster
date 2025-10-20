 #include "PlastibooMaterial.h"
  #include <iostream>

  namespace Plaster {

  PlastibooMaterial::PlastibooMaterial() {
      // Default material settings
      m_data.baseColor = glm::vec4(0.8f, 0.7f, 0.6f, 1.0f); // Clay tan
      m_data.clayRoughness = 0.75f;
      m_data.ditherStrength = 0.8f;
      m_data.warmthBias = 0.3f;
      m_data.paletteIndex = 0; // Medieval Dungeon
      m_data.useDithering = 1;
      m_data.useAffineMapping = 1; // PSX-style enabled by default
  }

  PlastibooMaterial::~PlastibooMaterial() {
  }

  bool PlastibooMaterial::create(VmaAllocator allocator) {
      // Create uniform buffer
      if (!m_uniformBuffer.create(
          allocator,
          sizeof(PlastibooMaterialData),
          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
          VMA_MEMORY_USAGE_CPU_TO_GPU,
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
      )) {
          std::cerr << "Failed to create material uniform buffer!" << std::endl;
          return false;
      }

      // Upload initial data
      updateData(allocator, m_data);

      std::cout << "PlastibooMaterial created" << std::endl;
      return true;
  }

  void PlastibooMaterial::destroy(VmaAllocator allocator) {
      m_uniformBuffer.destroy(allocator);
  }

  void PlastibooMaterial::updateData(VmaAllocator allocator, const PlastibooMaterialData& data) {
      m_data = data;
      m_uniformBuffer.copyData(allocator, &m_data, sizeof(PlastibooMaterialData));
  }

  PlastibooMaterialData PlastibooMaterial::createMedievalDungeonPreset() {
      PlastibooMaterialData data;
      data.baseColor = glm::vec4(0.45f, 0.38f, 0.32f, 1.0f); // Dark brown
      data.clayRoughness = 0.85f;
      data.ditherStrength = 0.9f;
      data.warmthBias = 0.25f;
      data.paletteIndex = 0; // Medieval Dungeon palette
      data.useDithering = 1;
      data.useAffineMapping = 1;
      return data;
  }

  PlastibooMaterialData PlastibooMaterial::createBloodRitualPreset() {
      PlastibooMaterialData data;
      data.baseColor = glm::vec4(0.6f, 0.15f, 0.12f, 1.0f); // Dark red
      data.clayRoughness = 0.8f;
      data.ditherStrength = 1.0f;
      data.warmthBias = 0.4f;
      data.paletteIndex = 4; // Blood Ritual palette
      data.useDithering = 1;
      data.useAffineMapping = 1;
      return data;
  }

  PlastibooMaterialData PlastibooMaterial::createPlagueVillagePreset() {
      PlastibooMaterialData data;
      data.baseColor = glm::vec4(0.55f, 0.52f, 0.35f, 1.0f); // Sickly yellow-green
      data.clayRoughness = 0.75f;
      data.ditherStrength = 0.85f;
      data.warmthBias = 0.2f;
      data.paletteIndex = 3; // Plague Village palette
      data.useDithering = 1;
      data.useAffineMapping = 1;
      return data;
  }

  PlastibooMaterialData PlastibooMaterial::createAncientForestPreset() {
      PlastibooMaterialData data;
      data.baseColor = glm::vec4(0.25f, 0.4f, 0.28f, 1.0f); // Forest green
      data.clayRoughness = 0.8f;
      data.ditherStrength = 0.75f;
      data.warmthBias = 0.15f;
      data.paletteIndex = 1; // Ancient Forest palette
      data.useDithering = 1;
      data.useAffineMapping = 1;
      return data;
  }

  }

 
