#include <iostream>
#include <stdexcept>
#include <memory>
#include "platform/Window.h"
#include "renderer/VulkanRenderer.h"
#include "scene/Scene.h"
#include "renderer/MeshPrimitives.h"

int main() {
    try {
        std::cout << "Plaster Engine - Plastiboo Rendering Test" << std::endl;

        // Create window
        Window window(1280, 720, "Plaster Engine - Plastiboo Demo");

        // Create Vulkan renderer
        VulkanRenderer renderer;
        renderer.initialize(window);

        std::cout << "Vulkan initialized successfully!" << std::endl;

        // Create scene
        Plaster::Scene scene;

        // Create meshes
        auto cubeMesh = std::make_shared<Plaster::Mesh>();
        auto sphereMesh = std::make_shared<Plaster::Mesh>();
        auto planeMesh = std::make_shared<Plaster::Mesh>();

        std::vector<Plaster::PlastibooVertex> vertices;
        std::vector<uint32_t> indices;

        // Create cube mesh
        Plaster::MeshPrimitives::createCube(vertices, indices, 1.0f, glm::vec3(0.8f, 0.7f, 0.6f));
        cubeMesh->create(renderer.getAllocator(), renderer.getDevice(),
                        renderer.getCommandPool(), renderer.getGraphicsQueue(),
                        vertices, indices);

        // Create sphere mesh
        vertices.clear();
        indices.clear();
        Plaster::MeshPrimitives::createSphere(vertices, indices, 0.8f, 16, 16, glm::vec3(0.9f, 0.8f, 0.75f));
        sphereMesh->create(renderer.getAllocator(), renderer.getDevice(),
                          renderer.getCommandPool(), renderer.getGraphicsQueue(),
                          vertices, indices);

        // Create ground plane
        vertices.clear();
        indices.clear();
        Plaster::MeshPrimitives::createPlane(vertices, indices, 15.0f, 15.0f, 10, 10, glm::vec3(0.4f, 0.38f, 0.35f));
        planeMesh->create(renderer.getAllocator(), renderer.getDevice(),
                         renderer.getCommandPool(), renderer.getGraphicsQueue(),
                         vertices, indices);

        // Create materials with different Plastiboo presets
        auto medievalMat = std::make_shared<Plaster::PlastibooMaterial>();
        medievalMat->create(renderer.getAllocator());
        medievalMat->updateData(renderer.getAllocator(),
                               Plaster::PlastibooMaterial::createMedievalDungeonPreset());

        auto bloodMat = std::make_shared<Plaster::PlastibooMaterial>();
        bloodMat->create(renderer.getAllocator());
        bloodMat->updateData(renderer.getAllocator(),
                            Plaster::PlastibooMaterial::createBloodRitualPreset());

        auto plagueMat = std::make_shared<Plaster::PlastibooMaterial>();
        plagueMat->create(renderer.getAllocator());
        plagueMat->updateData(renderer.getAllocator(),
                             Plaster::PlastibooMaterial::createPlagueVillagePreset());

        auto forestMat = std::make_shared<Plaster::PlastibooMaterial>();
        forestMat->create(renderer.getAllocator());
        forestMat->updateData(renderer.getAllocator(),
                             Plaster::PlastibooMaterial::createAncientForestPreset());

        // Add objects to scene
        scene.addObject(cubeMesh, medievalMat, glm::vec3(-2.5f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        scene.addObject(sphereMesh, bloodMat, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        scene.addObject(cubeMesh, plagueMat, glm::vec3(2.5f, 1.0f, 0.0f), glm::vec3(0.0f, 45.0f, 0.0f));
        scene.addObject(planeMesh, forestMat, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        // Position camera for a good view
        scene.getCamera().setPosition(glm::vec3(0.0f, 3.5f, 8.0f));
        scene.getCamera().lookAt(glm::vec3(0.0f, 1.0f, 0.0f));

        std::cout << "Scene created with " << scene.getObjects().size() << " objects" << std::endl;
        std::cout << "Rendering with Plastiboo materials:" << std::endl;
        std::cout << "  - Medieval Dungeon (left cube)" << std::endl;
        std::cout << "  - Blood Ritual (sphere)" << std::endl;
        std::cout << "  - Plague Village (right cube)" << std::endl;
        std::cout << "  - Ancient Forest (ground)" << std::endl;
        std::cout << "Close the window to exit." << std::endl;

        // Main loop
        float time = 0.0f;
        while (!window.shouldClose()) {
            window.pollEvents();

            // Simple animation: rotate objects
            time += 0.016f; // ~60 FPS
            auto& objects = scene.getObjects();
            if (objects.size() >= 3) {
                objects[0].rotation.y = time * 20.0f;  // Left cube
                objects[2].rotation.y = time * -15.0f; // Right cube
            }

            // Render the scene
            renderer.renderScene(scene);
            renderer.drawFrame();
        }

        // Cleanup materials
        medievalMat->destroy(renderer.getAllocator());
        bloodMat->destroy(renderer.getAllocator());
        plagueMat->destroy(renderer.getAllocator());
        forestMat->destroy(renderer.getAllocator());

        // Cleanup meshes
        cubeMesh->destroy(renderer.getAllocator());
        sphereMesh->destroy(renderer.getAllocator());
        planeMesh->destroy(renderer.getAllocator());

        std::cout << "Shutting down gracefully..." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
