#version 450


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inColor;

layout(set = 0, binding = 0) uniform CameraUBO {
  mat4 view;
  mat4 projection;
  vec3 cameraPos;
} camera;

layout(set = 1, binding = 0) uniform ObjectUBO {
  mat4 model;
  mat4 normalMatrix;
} object;

layout(location = 0 ) out vec3 fragWorldPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragColor;
layout(location = 4) out vec3 fragVertexLighting;
layout(location = 5) out float fragDepth;


layout(set = 0, binding = 1) uniform LightUBO {
  vec4 positions[4];
  vec4 colors[4];
  int numLights;
} lights;

vec3 snapToGrid(vec3 pos, float gridSize) {
  return floor(pos / gridSize) * gridSize;
}

vec3 calculateVertexLighting(vec3 worldPos, vec3 normal) {
  vec3 lighting = vec3(0.15, 0.12, 0.10);

  for(int i = 0; i < lights.numLights; i++) {
    vec3 lightPos = lights.positions[i].xyz;
    float lightIntensity = lights.positions[i].w;
    vec3 lightColor = lights.colors[i].rgb;

    vec3 L = normalize(lightPos - worldPos);
    float distance = length(lightPos - worldPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    float NdotL = dot(normal, L);
    float wrap = 0.5;
    float diffuse = (NdotL + wrap) / (1.0 + wrap);

    diffuse = max(diffuse, 0.0);
    lighting += lightColor * diffuse * attenuation * lightIntensity;
  }
  return lighting;
}

void main() {
  vec4 worldPos = object.model * vec4(inPosition, 1.0);
  fragWorldPos = worldPos.xyz;

  fragNormal = normalize(mat3(object.normalMatrix) * inNormal);
  
  float snapResolution = 0.05;
  vec3 snappedWorldPos = snapToGrid(fragWorldPos, snapResolution);
  
  fragVertexLighting = calculateVertexLighting(snappedWorldPos, fragNormal);
  
  vec4 clipPos = camera.projection * camera.view * vec4(snappedWorldPos, 1.0);

  fragDepth = clipPos.w;
  
  fragTexCoord = inTexCoord.xy * clipPos.w;
  
  fragColor = inColor;
  
  gl_Position = clipPos;
}


