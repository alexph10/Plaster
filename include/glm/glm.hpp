// Stub GLM header for basic compilation
#pragma once
#include <cmath>

#ifndef GLM_VERSION
#define GLM_VERSION 100

namespace glm {
// Basic vector types
struct vec2 {
  float x, y;
  vec2() : x(0), y(0) {}
  vec2(float x, float y) : x(x), y(y) {}
};

struct vec3 {
  float x, y, z;

  // Color access aliases
  float &r() { return x; }
  float &g() { return y; }
  float &b() { return z; }
  const float &r() const { return x; }
  const float &g() const { return y; }
  const float &b() const { return z; }

  vec3() : x(0), y(0), z(0) {}
  vec3(float x, float y, float z) : x(x), y(y), z(z) {}
  vec3(float v) : x(v), y(v), z(v) {}

  // Operators
  vec3 operator+(const vec3 &other) const {
    return vec3(x + other.x, y + other.y, z + other.z);
  }
  vec3 operator-(const vec3 &other) const {
    return vec3(x - other.x, y - other.y, z - other.z);
  }
  vec3 operator*(const vec3 &other) const {
    return vec3(x * other.x, y * other.y, z * other.z);
  }
  vec3 operator*(float f) const { return vec3(x * f, y * f, z * f); }
  vec3 &operator*=(const vec3 &other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    return *this;
  }
};

struct vec4 {
  float x, y, z, w;
  vec4() : x(0), y(0), z(0), w(0) {}
  vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
  vec4(float v) : x(v), y(v), z(v), w(v) {}
  vec4(const vec3 &v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
};

// Basic matrix type
struct mat4 {
  float data[16];
  mat4() {
    for (int i = 0; i < 16; i++)
      data[i] = 0.0f;
    data[0] = data[5] = data[10] = data[15] = 1.0f; // Identity
  }
  mat4(float f) {
    for (int i = 0; i < 16; i++)
      data[i] = 0.0f;
    data[0] = data[5] = data[10] = data[15] = f; // Diagonal matrix
  }
  const float *value_ptr() const { return data; }
};

// Basic functions
inline vec3 normalize(const vec3 &v) {
  float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (len > 0)
    return vec3(v.x / len, v.y / len, v.z / len);
  return vec3(0, 0, 0);
}

inline float dot(const vec3 &a, const vec3 &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float length(const vec3 &v) {
  return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline vec3 mix(const vec3 &a, const vec3 &b, float t) {
  return vec3(a.x * (1 - t) + b.x * t, a.y * (1 - t) + b.y * t,
              a.z * (1 - t) + b.z * t);
}

inline vec3 clamp(const vec3 &v, float min, float max) {
  return vec3(v.x < min ? min : (v.x > max ? max : v.x),
              v.y < min ? min : (v.y > max ? max : v.y),
              v.z < min ? min : (v.z > max ? max : v.z));
}

// Additional math functions
inline float radians(float degrees) {
  return degrees * 3.14159265359f / 180.0f;
}
inline mat4 perspective(float fovy, float aspect, float zNear, float zFar) {
  return mat4(1.0f);
}
inline mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
  return mat4(1.0f);
}
inline mat4 translate(const mat4 &m, const vec3 &v) {
  mat4 result = m;
  result.data[12] += v.x;
  result.data[13] += v.y;
  result.data[14] += v.z;
  return result;
}

} // namespace glm

#endif