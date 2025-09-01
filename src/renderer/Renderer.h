#pragma once
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <string>

class Shader;
class Camera;
class PostProcessor;
class Texture;
class Framebuffer;

struct PlastibooSettings {
    // Core aesthetic parameters
    int colorLevels = 6;              // Reduced for medieval palette
    float horizontalCrunch = 0.65f;   // More aggressive than standard 0.7f
    float grungeIntensity = 0.8f;     // Heavy texture overlay
    float depthFogDensity = 0.4f;     // Atmospheric depth
    
    // Medieval lighting
    glm::vec3 candlelightColor = glm::vec3(1.0f, 0.7f, 0.3f);
    float shadowHardness = 0.9f;      // Sharp shadows
    float ambientOcclusion = 0.6f;    // Deep shadows in corners
    
    // Horror atmosphere
    float vignetteStrength = 0.5f;    // Dark edges
    float scanlineIntensity = 0.3f;   // CRT effect
    bool enableFilmGrain = true;      // Analog noise
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Initialize();
    void Update(float deltaTime);
    void BeginFrame();
    void EndFrame();
    void Shutdown();

    // Enhanced Plastiboo rendering pipeline
    void BeginPlastibooPass(int lowResWidth = 320, int lowResHeight = 240);
    void EndPlastibooPass();
    void ApplyPlastibooPostProcessing(const PlastibooSettings& settings);
    
    // Material system for medieval assets
    void LoadMedievalTextures();
    void SetMedievalLighting(const glm::vec3& candlePos, float intensity);
    
    // Rendering functions
    void SetViewMatrix(const glm::mat4& view);
    void SetProjectionMatrix(const glm::mat4& projection);
    void SetRenderTarget(std::shared_ptr<Framebuffer> framebuffer);
    
    // Resource management
    void LoadTexture(const std::string& name, const std::string& path);
    std::shared_ptr<Texture> GetTexture(const std::string& name);
    
    // Camera integration
    void SetCamera(std::shared_ptr<Camera> camera);
    
private:
    // Core rendering state
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    
    // Enhanced pipeline components
    std::unique_ptr<PostProcessor> m_postProcessor;
    std::shared_ptr<Camera> m_currentCamera;
    
    // Plastiboo-specific framebuffers
    std::shared_ptr<Framebuffer> m_lowResFramebuffer;
    std::shared_ptr<Framebuffer> m_medievalLightingBuffer;
    std::shared_ptr<Framebuffer> m_grungeBuffer;
    
    // Material system
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
    
    // Plastiboo settings
    PlastibooSettings m_plastibooSettings;
    
    // Medieval lighting system
    struct MedievalLight {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        float flickerPhase;
    };
    std::vector<MedievalLight> m_medievalLights;
    
    // Performance monitoring
    float m_frameTime;
    int m_drawCalls;
    
    void InitializePlastibooShaders();
    void UpdateMedievalLighting(float deltaTime);
    void RenderDepthFog();
    void ApplyGrungeOverlay();
};

