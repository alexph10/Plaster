#include "MeshPrimitives.h"
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace Plaster {
namespace MeshPrimitives {
void createCube(
  std::vector<PlastibooVertex>& vertices,
  std::vector<uint32_t>& indices,
  float size,
  const glm::vec3& color
) {
  float half = size * 0.5f,

  vertices = {

    {{-half, -half, half}, {0, 0, 1}, {0, 0}, color},
    {{half, -half, half}, {0, 0, 1}, {1, 0}, color},
    {{half, half, half}, {0, 0, 1}, {1, 1}, color},
    {{-half, half, half}, {0, 0, 1}, {0, 1}, color},


    {{ half, -half, -half}, { 0,  0, -1}, {0, 0}, color},
    {{-half, -half, -half}, { 0,  0, -1}, {1, 0}, color},
    {{-half,  half, -half}, { 0,  0, -1}, {1, 1}, color},
    {{ half,  half, -half}, { 0,  0, -1}, {0, 1}, color},


    {{-half,  half,  half}, { 0,  1,  0}, {0, 0}, color},
    {{ half,  half,  half}, { 0,  1,  0}, {1, 0}, color},
    {{ half,  half, -half}, { 0,  1,  0}, {1, 1}, color},
    {{-half,  half, -half}, { 0,  1,  0}, {0, 1}, color},


    {{-half, -half, -half}, { 0, -1,  0}, {0, 0}, color},
    {{ half, -half, -half}, { 0, -1,  0}, {1, 0}, color},
    {{ half, -half,  half}, { 0, -1,  0}, {1, 1}, color},
    {{-half, -half,  half}, { 0, -1,  0}, {0, 1}, color},


    {{ half, -half,  half}, { 1,  0,  0}, {0, 0}, color},
    {{ half, -half, -half}, { 1,  0,  0}, {1, 0}, color},
    {{ half,  half, -half}, { 1,  0,  0}, {1, 1}, color},
    {{ half,  half,  half}, { 1,  0,  0}, {0, 1}, color},


    {{-half, -half, -half}, {-1,  0,  0}, {0, 0}, color},
    {{-half, -half,  half}, {-1,  0,  0}, {1, 0}, color},
    {{-half,  half,  half}, {-1,  0,  0}, {1, 1}, color},
    {{-half,  half, -half}, {-1,  0,  0}, {0, 1}, color}
  };
  indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
  };
}

void createSphere(
  std::vector<PlastibooVertex>& vertices,
  std::vector<uint32_t>& indices,
  float radius,
  int segments,
  int rings,
  const glm::vec3& color 
) {
  vertices.clear();
  indices.clear();

  const float pi = glm::pi<float>();

  for (int ring = 0; ring <= rings; ++ring) {
    float phi = pi * float(ring) / float(rings);
    float y = radius * cos(phi);
    float ringRadius = radius * sin(phi);

    for (int seg = 0; seg <= segmentsl; ++seg) {
      float theta = 2.0f * pi * float(seg) / float(segments);
      float x = ringRadius * cos(theta);
      float y = ringRadius * sin(theta);

      glm::vec3 position(x, y, z);
      glm::vec3 normal = glm::normalize(position);
      glm::vec2 texCoord(float(seg) / float(segments), float(ring) / float(rings));

      vertices.push_back({position, normal, texCoord, color});
    }
  }

  for (int ring = 0; ring < rings; ++ring) {
    for (int seg = 0; seg < segments; ++seg) {
      int current = ring * (segments + 1) + seg;
      int next = current + segments + 1;

      indices.push_back(current);
      indices.push_back(next);
      indices.push_back(current + 1);
      
      indices.push_back(current + 1);
      indices.push_back(next);
      indices.push_back(next + 1);
    }
  }
}

void createPlane(
  std::vector<PlastibooVertex>& vertices,
  std::vector<uint32_t>& indices,
  float width,
  float depth,
  int subdivisionsX,
  int subdivisionsZ,
  const glm::vec3& color 
) {
  vertices.clear();
  vertices.clear();

  float halfWidth = width * 0.5f;
  float halfDepth = depth * 0.5f;

  for (int z = 0; z <= subdivisionsZ; ++z) {
    for (int x = 0; x < subdivisionsX; ++x) {
      float xPos = -halfWidth + (width * float(x) / float(subdivisionsX));
      float zPos = -halfDepth + (depth * float(z) / float(subdivisionsZ));

      glm::vec3 position(xPos, 0.0f, zPos);
      glm::vec3 normal(0.0f, 1.0f, 0.0f);
      glm::vec2 texCoord(float(x) / float(subdivisionsX), float(z) / float(subdivisionsZ));

      vertices.push_back({position, normal, texCoord, color});
    }
  }

  for (int z = 0; z < subdivisionsZ; ++z) {
    for (int x = 0; x < subdivisionsX; ++x) {
      int topLeft = z * (subdivisionsX + 1) + x;
      int topRight = topLeft + 1;
      int bottomLeft = (z + 1) * (subdivisionsX + 1) + x;
      int bottomRight = bottomLeft + 1;


      indices.push_back(topLeft);
      indices.push_back(bottomLeft);
      indices.push_back(topRight);
      
      indices.push_back(topRight);
      indices.push_back(bottomLeft);
      indices.push_back(bottomRight);
    }
  }
}
}
}


