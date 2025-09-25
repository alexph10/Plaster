#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;
out vec3 VertexColor;
out float VertexDepth;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

// PS1-style rendering parameters
uniform float u_vertexSnapResolution = 160.0;    // Lower = more snapping
uniform float u_geometryJitter = 0.5;            // Vertex wobble strength
uniform float u_depthPrecisionLoss = 0.02;       // Z-fighting simulation
uniform bool u_enableAffineMapping = true;       // PS1-style texture mapping
uniform float u_morphingPhase = 0.0;             // For transformation effects

// Plastiboo-specific parameters
uniform float u_ancientDistortion = 0.0;         // Mysterious warping
uniform vec3 u_transformationCenter = vec3(0.0);  // Center of metamorphosis

vec3 ApplyVertexSnapping(vec3 pos, float resolution) {
    // Snap vertices to pixel grid like PS1
    vec3 snapped = pos;
    snapped.x = floor(snapped.x * resolution) / resolution;
    snapped.y = floor(snapped.y * resolution) / resolution;
    snapped.z = floor(snapped.z * resolution) / resolution;
    return snapped;
}

vec3 ApplyGeometryJitter(vec3 pos, float time) {
    // Subtle vertex jittering for that PS1 instability
    float jitter = u_geometryJitter * 0.001;
    pos.x += sin(time * 3.7 + pos.z * 100.0) * jitter;
    pos.y += cos(time * 4.1 + pos.x * 100.0) * jitter;
    pos.z += sin(time * 3.3 + pos.y * 100.0) * jitter;
    return pos;
}

vec3 ApplyAncientDistortion(vec3 pos, float phase) {
    if (u_ancientDistortion <= 0.0) return pos;
    
    // Create mysterious, organic warping effects
    vec3 toCenter = pos - u_transformationCenter;
    float dist = length(toCenter);
    
    // Breathing, alive distortion
    float breathe = sin(u_time * 2.0 + dist * 5.0) * 0.5 + 0.5;
    float warp = sin(u_time * 1.5 + pos.x * 3.0) * cos(u_time * 1.8 + pos.z * 3.0);
    
    pos += toCenter * warp * u_ancientDistortion * 0.1 * breathe;
    
    // Add transformation morphing
    if (u_morphingPhase > 0.0) {
        float morph = smoothstep(0.0, 1.0, u_morphingPhase);
        pos = mix(pos, pos + sin(pos * 10.0 + u_time) * 0.1, morph);
    }
    
    return pos;
}

void main() {
    vec3 worldPos = (u_model * vec4(aPos, 1.0)).xyz;
    
    // Apply Plastiboo-style distortions
    worldPos = ApplyAncientDistortion(worldPos, u_morphingPhase);
    worldPos = ApplyGeometryJitter(worldPos, u_time);
    
    // Transform to view space
    vec4 viewPos = u_view * vec4(worldPos, 1.0);
    
    // Apply vertex snapping in screen space for PS1 effect
    vec4 clipPos = u_projection * viewPos;
    clipPos.xyz = ApplyVertexSnapping(clipPos.xyz, u_vertexSnapResolution);
    
    // Simulate PS1 depth precision loss
    clipPos.z += sin(worldPos.x * 50.0 + worldPos.z * 50.0) * u_depthPrecisionLoss;
    
    gl_Position = clipPos;
    
    // Pass data to fragment shader
    WorldPos = worldPos;
    Normal = mat3(transpose(inverse(u_model))) * aNormal;
    VertexColor = aColor;
    VertexDepth = viewPos.z;
    
    // PS1-style affine texture mapping (perspective-incorrect)
    if (u_enableAffineMapping) {
        TexCoord = aTexCoord * clipPos.w;  // Remove perspective correction
    } else {
        TexCoord = aTexCoord;
    }
}