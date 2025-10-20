#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Plaster {

// Camera uniform buffer (matches shader set 0, binding 0)
struct CameraUBO {
   glm::mat4 view;
   glm::mat4 projection;
   glm::vec3 cameraPos;
   float padding;
};

// Light data (matches shader set 0, binding 1)
struct LightUBO {
   glm::vec4 positions[4];  // xyz = position, w = intensity
   glm::vec4 colors[4];     // rgb = color, a = unused
   int numLights;
   float padding[3];
};

// Per-object transform (matches shader set 1, binding 0)
struct ObjectUBO {
   glm::mat4 model;
   glm::mat4 normalMatrix;
};

// Helper to create warm horror lighting
inline LightUBO createPlastibooLighting() {
   LightUBO lights{};
   lights.numLights = 2;

   // Key light - warm orange (torch-like)
   lights.positions[0] = glm::vec4(3.0f, 4.0f, 3.0f, 1.5f); // position + intensity
   lights.colors[0] = glm::vec4(1.0f, 0.65f, 0.3f, 1.0f);   // Orange

   // Fill light - cool desaturated (ambient moonlight)
   lights.positions[1] = glm::vec4(-2.0f, 2.0f, -2.0f, 0.4f);
   lights.colors[1] = glm::vec4(0.5f, 0.55f, 0.6f, 1.0f);   // Cool gray-blue

   return lights;
}
}