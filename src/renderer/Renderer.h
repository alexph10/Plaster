#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>


class Shader;
class Camera;
class PostProcessor;
class Texture;
class Framebuffer;

struct PlastibooSettings {
  // Core aesthetic parameters
  int colorLevels = 8;            // Enhanced palette size
  float horizontalCrunch = 0.65f; // More aggressive than standard 0.7f
  float grungeIntensity = 0.8f;   // Heavy texture overlay
  float depthFogDensity = 0.4f;   // Atmospheric depth

  // PS1-style rendering
  float vertexSnapEffect = 0.7f;    // Vertex snapping strength
  float paletteReduction = 0.8f;    // Color quantization intensity
  bool enableAffineMapping = true;  // PS1-style texture mapping
  float depthPrecisionLoss = 0.02f; // Z-fighting simulation

  // Organic, breathing effects
  float organicBreathing = 0.3f;      // Living surface effect
  float breathingSpeed = 2.0f;        // Animation speed
  float organicDetailStrength = 0.7f; // Detail layer intensity

  // Horror atmosphere
  float horrorAtmosphere = 0.5f;  // Overall unsettling factor
  float ancientDistortion = 0.2f; // Mysterious warping
  float vignetteStrength = 0.5f;  // Dark edges
  float scanlineIntensity = 0.3f; // CRT effect
  bool enableFilmGrain = true;    // Analog noise

  // Dynamic palette system
  int paletteType = 0; // 0=medieval, 1=forest, 2=monastery, 3=plague
  float paletteTransitionSpeed = 1.0f;
  bool enablePaletteBreathing = true;

  // Medieval lighting (enhanced)
  glm::vec3 candlelightColor = glm::vec3(1.0f, 0.7f, 0.3f);
  float shadowHardness = 0.9f;   // Sharp shadows
  float ambientOcclusion = 0.6f; // Deep shadows in corners
  float ancientGlow = 0.2f;      // Mysterious luminance

  // Transformation effects
  float morphingPhase = 0.0f; // 0.0 to 1.0 transformation progress
  glm::vec3 transformationCenter = glm::vec3(0.0f);
  float geometryJitter = 0.5f; // Vertex instability
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
  void ApplyPlastibooPostProcessing(const PlastibooSettings &settings);

  // Organic detail generation
  void GenerateOrganicNoiseTexture();
  void UpdateOrganicEffects(float deltaTime);

  // Palette management
  void SetPlastibooPalette(int paletteType);
  void TransitionToPalette(int targetPalette, float duration);

  // Transformation effects
  void SetMorphingEffect(float phase, const glm::vec3 &center);
  void EnableBreathingEffect(bool enable, float intensity = 0.3f);

  // Material system for medieval assets
  void LoadMedievalTextures();
  void SetMedievalLighting(const glm::vec3 &candlePos, float intensity);

  // Rendering functions
  void SetViewMatrix(const glm::mat4 &view);
  void SetProjectionMatrix(const glm::mat4 &projection);
  void SetRenderTarget(std::shared_ptr<Framebuffer> framebuffer);

  // Resource management
  void LoadTexture(const std::string &name, const std::string &path);
  std::shared_ptr<Texture> GetTexture(const std::string &name);

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
