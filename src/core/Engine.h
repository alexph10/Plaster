#pragma once

#include <memory>
#include <vector>

// Forward declarations
class Renderer;
class AudioSystem;
class InputManager;

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize();
    void Update();
    void Render();
    void Shutdown();

    // Getters for subsystems
    Renderer* GetRenderer() const { return m_renderer.get(); }
    AudioSystem* GetAudioSystem() const { return m_audioSystem.get(); }
    InputManager* GetInputManager() const { return m_inputManager.get(); }

private:
    // Core subsystems
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<AudioSystem> m_audioSystem;
    std::unique_ptr<InputManager> m_inputManager;

    // Engine state
    bool m_isRunning;
    float m_deltaTime;
    float m_lastFrameTime;

    void CalculateDeltaTime();
};
