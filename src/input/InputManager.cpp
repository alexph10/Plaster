#include "InputManager.h"
#include <spdlog/spdlog.h>

InputManager* InputManager::s_instance = nullptr;

InputManager::InputManager() 
    : m_window(nullptr), m_mouseX(0.0), m_mouseY(0.0), m_firstMouse(true) {
    s_instance = this;
}

InputManager::~InputManager() {
    s_instance = nullptr;
}

bool InputManager::Initialize() {
    spdlog::info("Input Manager initialized");
    return true;
}

void InputManager::Update() {
    for (auto& pair : m_keyStates) {
        if (pair.second == KeyState::Pressed) {
            pair.second = KeyState::Held;
        }
    }
}

void InputManager::Shutdown() {
    m_keyStates.clear();
}

KeyState InputManager::GetKeyState(int key) const {
    auto it = m_keyStates.find(key);
    return (it != m_keyStates.end()) ? it->second : KeyState::Released;
}

bool InputManager::IsKeyPressed(int key) const {
    return GetKeyState(key) == KeyState::Pressed;
}

void InputManager::SetWindow(GLFWwindow* window) {
    m_window = window;
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, KeyCallback);
}

void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (s_instance) {
        s_instance->UpdateKeyState(key, action);
    }
}

void InputManager::UpdateKeyState(int key, int action) {
    if (action == GLFW_PRESS) {
        m_keyStates[key] = KeyState::Pressed;
    } else if (action == GLFW_RELEASE) {
        m_keyStates[key] = KeyState::Released;
    }
}
