#include "PlastibooPalette.h"
#include <algorithm>
#include <cmath>
#include <random>

PlastibooPalette::PlastibooPalette()
    : m_currentType(PlastibooPaletteType::MEDIEVAL_DUNGEON),
      m_paletteShift(1.0f, 1.0f, 1.0f), m_paletteDistortion(0.0f),
      m_breathingEnabled(false), m_breathingIntensity(0.1f),
      m_breathingPhase(0.0f), m_isTransitioning(false),
      m_transitionProgress(0.0f), m_transitionDuration(1.0f),
      m_corruptionLevel(0.0f), m_ancientGlowLevel(0.0f) {
  InitializePredefinedPalettes();
  SetPaletteType(PlastibooPaletteType::MEDIEVAL_DUNGEON);
}

void PlastibooPalette::InitializePredefinedPalettes() {
  // Medieval Dungeon - Dark, oppressive tones
  m_predefinedPalettes[PlastibooPaletteType::MEDIEVAL_DUNGEON] = {
      {{0.08f, 0.06f, 0.04f}, 0.3f, 0.1f, "Deep Shadow"},
      {{0.15f, 0.12f, 0.08f}, 0.25f, 0.15f, "Dark Stone"},
      {{0.25f, 0.20f, 0.12f}, 0.2f, 0.2f, "Worn Stone"},
      {{0.40f, 0.32f, 0.20f}, 0.15f, 0.25f, "Old Wood"},
      {{0.55f, 0.45f, 0.25f}, 0.08f, 0.3f, "Torch Light"},
      {{0.12f, 0.15f, 0.08f}, 0.02f, 0.15f, "Moss Shadow"}};

  // Ancient Forest - Deep greens with earth tones
  m_predefinedPalettes[PlastibooPaletteType::ANCIENT_FOREST] = {
      {{0.05f, 0.08f, 0.05f}, 0.3f, 0.1f, "Deep Forest Shadow"},
      {{0.12f, 0.18f, 0.10f}, 0.25f, 0.15f, "Dark Moss"},
      {{0.20f, 0.28f, 0.15f}, 0.2f, 0.2f, "Forest Green"},
      {{0.35f, 0.25f, 0.15f}, 0.15f, 0.25f, "Tree Bark"},
      {{0.45f, 0.35f, 0.20f}, 0.08f, 0.3f, "Autumn Light"},
      {{0.25f, 0.35f, 0.20f}, 0.02f, 0.15f, "Sage Green"}};

  // Cursed Monastery - Cold, ethereal tones
  m_predefinedPalettes[PlastibooPaletteType::CURSED_MONASTERY] = {
      {{0.08f, 0.06f, 0.12f}, 0.3f, 0.1f, "Purple Shadow"},
      {{0.15f, 0.12f, 0.18f}, 0.25f, 0.15f, "Cold Stone"},
      {{0.22f, 0.20f, 0.25f}, 0.2f, 0.2f, "Ghostly Gray"},
      {{0.35f, 0.30f, 0.40f}, 0.15f, 0.25f, "Pale Purple"},
      {{0.50f, 0.45f, 0.55f}, 0.08f, 0.3f, "Ethereal Light"},
      {{0.18f, 0.15f, 0.25f}, 0.02f, 0.15f, "Dark Violet"}};

  // Plague Village - Sickly, diseased colors
  m_predefinedPalettes[PlastibooPaletteType::PLAGUE_VILLAGE] = {
      {{0.12f, 0.10f, 0.06f}, 0.3f, 0.1f, "Diseased Shadow"},
      {{0.22f, 0.18f, 0.08f}, 0.25f, 0.15f, "Sickly Yellow"},
      {{0.30f, 0.25f, 0.12f}, 0.2f, 0.2f, "Rotting Wood"},
      {{0.18f, 0.22f, 0.08f}, 0.15f, 0.25f, "Plague Green"},
      {{0.35f, 0.30f, 0.15f}, 0.08f, 0.3f, "Feverish Light"},
      {{0.25f, 0.20f, 0.10f}, 0.02f, 0.15f, "Decay Brown"}};

  // Blood Ritual - Deep reds and dark shadows
  m_predefinedPalettes[PlastibooPaletteType::BLOOD_RITUAL] = {
      {{0.08f, 0.04f, 0.04f}, 0.3f, 0.1f, "Blood Shadow"},
      {{0.18f, 0.08f, 0.06f}, 0.25f, 0.15f, "Dark Red"},
      {{0.30f, 0.12f, 0.10f}, 0.2f, 0.2f, "Dried Blood"},
      {{0.45f, 0.15f, 0.12f}, 0.15f, 0.25f, "Fresh Blood"},
      {{0.35f, 0.25f, 0.20f}, 0.08f, 0.3f, "Rust Red"},
      {{0.12f, 0.06f, 0.08f}, 0.02f, 0.15f, "Crimson Shadow"}};

  // Add more palettes as needed...
}

void PlastibooPalette::SetPaletteType(PlastibooPaletteType type) {
  if (m_predefinedPalettes.find(type) == m_predefinedPalettes.end()) {
    return; // Palette not found
  }

  m_currentType = type;
  const auto &paletteColors = m_predefinedPalettes[type];

  m_currentPalette.clear();
  m_detailedPalette = paletteColors;

  // Extract just the RGB values for easy shader access
  for (const auto &color : paletteColors) {
    m_currentPalette.push_back(color.rgb);
  }
}

void PlastibooPalette::UpdatePalette(float deltaTime) {
  if (m_breathingEnabled) {
    m_breathingPhase += deltaTime * 2.0f;

    // Apply breathing effect to palette shift
    float breathe = std::sin(m_breathingPhase) * m_breathingIntensity + 1.0f;
    m_paletteShift = glm::vec3(breathe, breathe * 0.95f, breathe * 0.9f);
  }

  // Update transitions
  if (m_isTransitioning) {
    UpdateTransition(deltaTime);
  }

  // Apply corruption effects over time
  if (m_corruptionLevel > 0.0f) {
    ApplyCorruptionEffect(m_corruptionLevel);
  }
}

glm::vec3 PlastibooPalette::QuantizeColor(const glm::vec3 &color) const {
  glm::vec3 processedColor = ApplyColorEffects(color);

  float minDistance = 1000.0f;
  glm::vec3 nearestColor = m_currentPalette[0];

  for (const auto &paletteColor : m_currentPalette) {
    float distance = glm::length(processedColor - paletteColor);
    if (distance < minDistance) {
      minDistance = distance;
      nearestColor = paletteColor;
    }
  }

  return nearestColor;
}

glm::vec3 PlastibooPalette::QuantizeColorWithDither(const glm::vec3 &color,
                                                    float ditherAmount) const {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

  // Add dither noise
  glm::vec3 ditheredColor = color + glm::vec3(dis(gen) * ditherAmount * 0.1f,
                                              dis(gen) * ditherAmount * 0.1f,
                                              dis(gen) * ditherAmount * 0.1f);

  return QuantizeColor(ditheredColor);
}

void PlastibooPalette::StartPaletteTransition(
    PlastibooPaletteType targetPalette, float duration) {
  if (m_currentType == targetPalette)
    return;

  m_sourcePalette = m_currentType;
  m_targetPalette = targetPalette;
  m_transitionDuration = duration;
  m_transitionProgress = 0.0f;
  m_isTransitioning = true;
}

void PlastibooPalette::UpdateTransition(float deltaTime) {
  if (!m_isTransitioning)
    return;

  m_transitionProgress += deltaTime / m_transitionDuration;

  if (m_transitionProgress >= 1.0f) {
    m_transitionProgress = 1.0f;
    m_isTransitioning = false;
    SetPaletteType(m_targetPalette);
    return;
  }

  // Smooth transition curve
  float t = m_transitionProgress;
  t = t * t * (3.0f - 2.0f * t); // Smoothstep

  // Interpolate between source and target palettes
  if (m_predefinedPalettes.find(m_sourcePalette) !=
          m_predefinedPalettes.end() &&
      m_predefinedPalettes.find(m_targetPalette) !=
          m_predefinedPalettes.end()) {

    InterpolatePalettes(m_predefinedPalettes[m_sourcePalette],
                        m_predefinedPalettes[m_targetPalette], t);
  }
}

void PlastibooPalette::InterpolatePalettes(
    const std::vector<PlastibooColor> &source,
    const std::vector<PlastibooColor> &target, float t) {
  m_currentPalette.clear();

  size_t maxSize = std::max(source.size(), target.size());

  for (size_t i = 0; i < maxSize; ++i) {
    glm::vec3 sourceColor =
        i < source.size() ? source[i].rgb : source.back().rgb;
    glm::vec3 targetColor =
        i < target.size() ? target[i].rgb : target.back().rgb;

    glm::vec3 interpolatedColor = glm::mix(sourceColor, targetColor, t);
    m_currentPalette.push_back(interpolatedColor);
  }
}

void PlastibooPalette::EnablePaletteBreathing(bool enable, float intensity) {
  m_breathingEnabled = enable;
  m_breathingIntensity = intensity;
  if (!enable) {
    m_paletteShift = glm::vec3(1.0f);
    m_breathingPhase = 0.0f;
  }
}

void PlastibooPalette::ApplyCorruptionEffect(float corruption) {
  m_corruptionLevel = std::clamp(corruption, 0.0f, 1.0f);

  // Corruption shifts colors toward darker, more unsettling tones
  float corruptionShift = 1.0f - m_corruptionLevel * 0.3f;
  m_paletteShift *= glm::vec3(corruptionShift, corruptionShift * 0.8f,
                              corruptionShift * 0.6f);
}

void PlastibooPalette::ApplyAncientGlow(float glowIntensity) {
  m_ancientGlowLevel = std::clamp(glowIntensity, 0.0f, 1.0f);

  // Ancient glow adds mysterious luminance to certain colors
  float glowBoost = 1.0f + m_ancientGlowLevel * 0.2f;
  m_paletteShift.y *= glowBoost; // Boost green channel for mystical effect
}

glm::vec3 PlastibooPalette::ApplyColorEffects(const glm::vec3 &color) const {
  glm::vec3 result = color * m_paletteShift;

  // Apply palette distortion for surreal effects
  if (m_paletteDistortion > 0.0f) {
    glm::vec3 hsv = RGBToHSV(result);
    hsv.x += std::sin(hsv.y * 10.0f) * m_paletteDistortion * 0.1f; // Hue shift
    hsv.y *= (1.0f + std::cos(hsv.z * 5.0f) * m_paletteDistortion *
                         0.2f); // Saturation warp
    result = HSVToRGB(hsv);
  }

  return glm::clamp(result, 0.0f, 1.0f);
}

// Color space conversion utilities
glm::vec3 PlastibooPalette::RGBToHSV(const glm::vec3 &rgb) const {
  float r = rgb.r, g = rgb.g, b = rgb.b;
  float max = std::max({r, g, b});
  float min = std::min({r, g, b});
  float delta = max - min;

  glm::vec3 hsv;
  hsv.z = max; // Value

  if (max == 0.0f) {
    hsv.y = hsv.x = 0.0f; // Saturation, Hue
    return hsv;
  }

  hsv.y = delta / max; // Saturation

  if (delta == 0.0f) {
    hsv.x = 0.0f; // Hue
  } else if (max == r) {
    hsv.x = (g - b) / delta + (g < b ? 6.0f : 0.0f);
  } else if (max == g) {
    hsv.x = (b - r) / delta + 2.0f;
  } else {
    hsv.x = (r - g) / delta + 4.0f;
  }

  hsv.x /= 6.0f;
  return hsv;
}

glm::vec3 PlastibooPalette::HSVToRGB(const glm::vec3 &hsv) const {
  float h = hsv.x * 6.0f;
  float s = hsv.y;
  float v = hsv.z;

  int i = (int)h;
  float f = h - i;
  float p = v * (1.0f - s);
  float q = v * (1.0f - s * f);
  float t = v * (1.0f - s * (1.0f - f));

  switch (i % 6) {
  case 0:
    return glm::vec3(v, t, p);
  case 1:
    return glm::vec3(q, v, p);
  case 2:
    return glm::vec3(p, v, t);
  case 3:
    return glm::vec3(p, q, v);
  case 4:
    return glm::vec3(t, p, v);
  case 5:
    return glm::vec3(v, p, q);
  default:
    return glm::vec3(0.0f);
  }
}