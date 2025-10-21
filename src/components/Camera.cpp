#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Plaster {

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
    : m_position(0.0f, 0.0f, 3.0f)
    , m_pitch(0.0f)
    , m_yaw(-90.0f)  // Point forward along -Z
    , m_fov(fov)
    , m_aspectRatio(aspectRatio)
    , m_nearPlane(nearPlane)
    , m_farPlane(farPlane)
{
    updateVectors();
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
}

void Camera::setRotation(float pitch, float yaw) {
    m_pitch = pitch;
    m_yaw = yaw;

    // Constrain pitch to avoid gimbal lock
    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateVectors();
}

void Camera::lookAt(const glm::vec3& target, const glm::vec3& up) {
    glm::vec3 direction = glm::normalize(target - m_position);

    // Calculate pitch and yaw from direction
    m_pitch = glm::degrees(asin(direction.y));
    m_yaw = glm::degrees(atan2(direction.z, direction.x));

    updateVectors();
}

void Camera::move(const glm::vec3& offset) {
    m_position += offset;
}

void Camera::rotate(float pitchDelta, float yawDelta) {
    m_pitch += pitchDelta;
    m_yaw += yawDelta;

    // Constrain pitch
    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::setAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
}

void Camera::setFOV(float fov) {
    m_fov = fov;
}

void Camera::updateVectors() {
    // Calculate forward vector from pitch and yaw
    glm::vec3 forward;
    forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    forward.y = sin(glm::radians(m_pitch));
    forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward = glm::normalize(forward);

    // Calculate right and up vectors
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}

}
