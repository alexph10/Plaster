#pragma once

#include "../renderer/Mesh.h"
#include "../renderer/PlastibooMaterial.h"
#include "../renderer/UniformBuffers.h"
#include "../components/Camera.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace Plaster {

struct RenderObject {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<PlastibooMaterial> material;
    glm::vec3 position;
    glm::vec3 rotation;  // Euler angles in degrees
    glm::vec3 scale;

    RenderObject()
        : position(0.0f)
        , rotation(0.0f)
        , scale(1.0f)
    {}

    glm::mat4 getModelMatrix() const;
    glm::mat4 getNormalMatrix() const;
};

class Scene {
public:
    Scene();
    ~Scene() = default;

    // Object management
    void addObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<PlastibooMaterial> material,
                   const glm::vec3& position = glm::vec3(0.0f),
                   const glm::vec3& rotation = glm::vec3(0.0f),
                   const glm::vec3& scale = glm::vec3(1.0f));

    std::vector<RenderObject>& getObjects() { return m_objects; }
    const std::vector<RenderObject>& getObjects() const { return m_objects; }

    // Camera access
    Camera& getCamera() { return m_camera; }
    const Camera& getCamera() const { return m_camera; }

    // Lighting
    LightUBO& getLights() { return m_lights; }
    const LightUBO& getLights() const { return m_lights; }

    void setLights(const LightUBO& lights) { m_lights = lights; }

private:
    std::vector<RenderObject> m_objects;
    Camera m_camera;
    LightUBO m_lights;
};

}
