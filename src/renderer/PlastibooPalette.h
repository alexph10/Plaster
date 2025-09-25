#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>


// Forward declarations
struct PlastibooColor {
  glm::vec3 rgb;
  float weight;
  float variance;
  std::string name;
};

// Hash function for enum class
template <> struct std::hash<enum class PlastibooPaletteType> {
  size_t operator()(const enum class PlastibooPaletteType &t) const {
    return std::hash<int>{}(static_cast<int>(t));
  }
};
enum class PlastibooPaletteType {
  MEDIEVAL_DUNGEON, // Dark browns, muted greens
  ANCIENT_FOREST,   // Deep greens, earthy browns
  CURSED_MONASTERY, // Purple shadows, cold stones
  PLAGUE_VILLAGE,   // Sickly yellows, rotting greens
  BLOOD_RITUAL,     // Deep reds, black shadows
  ETHEREAL_VOID,    // Pale blues, ghostly whites
  COPPER_RUINS,     // Rust oranges, aged metals
  NIGHTMARE_REALM,  // Shifting, unstable colors
  CUSTOM            // User-defined palette
};

struct PlastibooColor {
  glm::vec3 rgb;
  float weight;     // How often this color appears in palette selection
  float variance;   // How much this color can shift
  std::string name; // For debugging/UI
};

class PlastibooPalette {
public:
  PlastibooPalette();
  ~PlastibooPalette() = default;

  // Palette management
  void SetPaletteType(PlastibooPaletteType type);
  void CreateCustomPalette(const std::vector<glm::vec3> &colors);
  void BlendPalettes(PlastibooPaletteType primary,
                     PlastibooPaletteType secondary, float blend);

  // Dynamic palette effects
  void UpdatePalette(float deltaTime);
  void ShiftPaletteHue(float hueShift);
  void ModifyPaletteSaturation(float saturation);
  void ApplyPaletteDistortion(float distortionAmount);

  // Color quantization
  glm::vec3 QuantizeColor(const glm::vec3 &color) const;
  glm::vec3 QuantizeColorWithDither(const glm::vec3 &color,
                                    float ditherAmount) const;

  // Palette animation for transformation effects
  void StartPaletteTransition(PlastibooPaletteType targetPalette,
                              float duration);
  void UpdateTransition(float deltaTime);
  bool IsTransitioning() const { return m_isTransitioning; }

  // Getters for shader uniforms
  const std::vector<glm::vec3> &GetCurrentPalette() const {
    return m_currentPalette;
  }
  glm::vec3 GetPaletteShift() const { return m_paletteShift; }
  float GetPaletteDistortion() const { return m_paletteDistortion; }

  // Palette breathing effect (makes colors feel alive)
  void EnablePaletteBreathing(bool enable, float intensity = 0.1f);

  // Horror-specific effects
  void ApplyCorruptionEffect(float corruption); // Gradually corrupts palette
  void ApplyAncientGlow(float glowIntensity);   // Adds mysterious luminance

  // Utility functions
  void SavePalette(const std::string &name);
  bool LoadPalette(const std::string &name);
  void ResetPalette();

private:
  PlastibooPaletteType m_currentType;
  std::vector<glm::vec3> m_currentPalette;
  std::vector<PlastibooColor> m_detailedPalette;

  // Dynamic effects
  glm::vec3 m_paletteShift;
  float m_paletteDistortion;
  bool m_breathingEnabled;
  float m_breathingIntensity;
  float m_breathingPhase;

  // Transition system
  bool m_isTransitioning;
  PlastibooPaletteType m_sourcePalette;
  PlastibooPaletteType m_targetPalette;
  float m_transitionProgress;
  float m_transitionDuration;

  // Horror effects
  float m_corruptionLevel;
  float m_ancientGlowLevel;

  // Predefined palettes
  std::unordered_map<PlastibooPaletteType, std::vector<PlastibooColor>>
      m_predefinedPalettes;
  std::unordered_map<std::string, std::vector<glm::vec3>> m_savedPalettes;

  void InitializePredefinedPalettes();
  void InterpolatePalettes(const std::vector<PlastibooColor> &source,
                           const std::vector<PlastibooColor> &target, float t);
  glm::vec3 FindNearestPaletteColor(const glm::vec3 &color,
                                    float &distance) const;
  glm::vec3 ApplyColorEffects(const glm::vec3 &color) const;

  // Color space utilities
  glm::vec3 RGBToHSV(const glm::vec3 &rgb) const;
  glm::vec3 HSVToRGB(const glm::vec3 &hsv) const;
};