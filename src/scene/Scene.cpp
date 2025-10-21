#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Plaster {

glm::mat4 RenderObject::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);

    // Apply transformations: translate, rotate, scale (TRS order)
    model = glm::translate(model, position);

    // Apply rotations in YXZ order (common for game engines)
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    return model;
}

glm::mat4 RenderObject::getNormalMatrix() const {
    glm::mat4 model = getModelMatrix();
    return glm::transpose(glm::inverse(model));
}

Scene::Scene()
    : m_camera(60.0f, 16.0f / 9.0f, 0.1f, 100.0f)
{
    // Initialize with default Plastiboo lighting
    m_lights = createPlastibooLighting();

    // Position camera for a good initial view
    m_camera.setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
    m_camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Scene::addObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<PlastibooMaterial> material,
                      const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    RenderObject obj;
    obj.mesh = mesh;
    obj.material = material;
    obj.position = position;
    obj.rotation = rotation;
    obj.scale = scale;

    m_objects.push_back(obj);
}

}
