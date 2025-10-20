#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>

namespace Plaster {
namespace MeshPrimitives {
void createCube(
  std::vector<PlastibooVertex>& vertices,
  std::vector<uint32_t>& indices,
  float size = 1.0f,
  const glm::vec3& color = glm::vec3(0.8f, 0.7f, 0.6f)
);
void createSphere(
  std::vector<PlastibooVertex>& vertices,
  std::vector<uint32_t>& indices,
  float radius = 1.0f,
  int segments = 16, 
  int rings = 16,
  const glm::vec3& color = glm::vec3(0.8f, 0.7f, 0.6f)
);

void createPlane(
  std::vector<PlastibooVertex>& vertices,
  std::vector<uint32_t>& indices,
  float width = 10.0f,
  float depth = 10.0f,
  int subdevisionsX = 10, 
  int subdivisionsZ = 10, 
  const glm::vec3& color = glm::vec3(0.5f, 0.45f, 0.4f)
);
}
}


