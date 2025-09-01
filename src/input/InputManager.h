#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>

enum class KeyState {
    Released,
    Pressed,
    Held
};

class InputManager {
public:
    InputManager();
    ~InputManager();

    bool Initialize();
    void Update();
    void Shutdown();


    KeyState GetKeyState(int key) const;
    bool IsKeyPressed(int key) const;
    bool IsKeyHeld(int key) const;
    bool IsKeyReleased(int key) const;


    KeyState GetMouseButtonState(int button) const;
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonHeld(int button) const;
    bool IsMouseButtonReleased(int button) const;

    void GetMousePosition(double& x, double& y) const;
    void GetMouseDelta(double& deltaX, double& deltaY) const;
    void SetMousePosition(double x, double y);
    void SetCursorMode(int mode); 


    void SetWindow(GLFWwindow* window);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

private:
    GLFWwindow* m_window;
    

    std::unordered_map<int, KeyState> m_keyStates;
    std::unordered_map<int, KeyState> m_mouseButtonStates;
    

    double m_mouseX, m_mouseY;
    double m_lastMouseX, m_lastMouseY;
    double m_mouseDeltaX, m_mouseDeltaY;
    bool m_firstMouse;


    static InputManager* s_instance;

    void UpdateKeyState(int key, int action);
    void UpdateMouseButtonState(int button, int action);
    void UpdateMousePosition(double xpos, double ypos);
};
