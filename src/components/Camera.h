#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Plaster {

class Camera {
public:
    Camera(float fov = 60.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 100.0f);
    ~Camera() = default;

    // Camera movement
    void setPosition(const glm::vec3& position);
    void setRotation(float pitch, float yaw);
    void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

    // Camera controls for free-fly
    void move(const glm::vec3& offset);
    void rotate(float pitchDelta, float yawDelta);

    // Matrix getters
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getForward() const { return m_forward; }
    glm::vec3 getRight() const { return m_right; }
    glm::vec3 getUp() const { return m_up; }

    // Projection parameters
    void setAspectRatio(float aspectRatio);
    void setFOV(float fov);

private:
    void updateVectors();

    // Camera transform
    glm::vec3 m_position;
    float m_pitch;  // Rotation around right axis
    float m_yaw;    // Rotation around up axis

    // Camera vectors
    glm::vec3 m_forward;
    glm::vec3 m_right;
    glm::vec3 m_up;

    // Projection parameters
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
};

}
