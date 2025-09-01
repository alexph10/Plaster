#include "Engine.h"
#include "renderer/Renderer.h"
#include "audio/AudioSystem.h"
#include "input/InputManager.h"

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

Engine::Engine() 
    : m_isRunning(false)
    , m_deltaTime(0.0f)
    , m_lastFrameTime(0.0f) {
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize() {
    spdlog::info("Initializing Engine subsystems...");


    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->Initialize()) {
        spdlog::error("Failed to initialize Renderer");
        return false;
    }


    m_audioSystem = std::make_unique<AudioSystem>();
    if (!m_audioSystem->Initialize()) {
        spdlog::error("Failed to initialize AudioSystem");
        return false;
    }


    m_inputManager = std::make_unique<InputManager>();
    if (!m_inputManager->Initialize()) {
        spdlog::error("Failed to initialize InputManager");
        return false;
    }

    m_isRunning = true;
    m_lastFrameTime = static_cast<float>(glfwGetTime());

    spdlog::info("Engine initialization complete");
    return true;
}

void Engine::Update() {
    if (!m_isRunning) return;

    CalculateDeltaTime();


    m_inputManager->Update();
    m_audioSystem->Update(m_deltaTime);
    m_renderer->Update(m_deltaTime);
}

void Engine::Render() {
    if (!m_isRunning) return;

    m_renderer->BeginFrame();
    

    
    m_renderer->EndFrame();
}

void Engine::Shutdown() {
    if (!m_isRunning) return;

    spdlog::info("Shutting down Engine...");

    m_inputManager.reset();
    m_audioSystem.reset();
    m_renderer.reset();

    m_isRunning = false;
}

void Engine::CalculateDeltaTime() {
    float currentTime = static_cast<float>(glfwGetTime());
    m_deltaTime = currentTime - m_lastFrameTime;
    m_lastFrameTime = currentTime;
}
