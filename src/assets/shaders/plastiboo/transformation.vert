#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;
out float TransformationFactor;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_time;

// Transformation parameters
uniform float u_morphingPhase = 0.0;      // 0.0 to 1.0 transformation progress
uniform vec3 u_transformationCenter = vec3(0.0);
uniform float u_transformationRadius = 5.0;
uniform int u_transformationType = 0;     // 0=organic, 1=crystalline, 2=dissolution

// Ancient distortion
uniform float u_ancientDistortion = 0.0;
uniform float u_cosmicInfluence = 0.0;

// Simple 3D noise for transformation effects
float hash(vec3 p) {
    p = fract(p * vec3(443.8975, 397.2973, 491.1871));
    p += dot(p, p.yxz + 19.19);
    return fract((p.x + p.y) * p.z);
}

float noise3D(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    return mix(mix(mix(hash(i + vec3(0, 0, 0)), hash(i + vec3(1, 0, 0)), f.x),
                   mix(hash(i + vec3(0, 1, 0)), hash(i + vec3(1, 1, 0)), f.x), f.y),
               mix(mix(hash(i + vec3(0, 0, 1)), hash(i + vec3(1, 0, 1)), f.x),
                   mix(hash(i + vec3(0, 1, 1)), hash(i + vec3(1, 1, 1)), f.x), f.y), f.z);
}

vec3 ApplyOrganicTransformation(vec3 pos, float phase) {
    if (phase <= 0.0) return pos;
    
    vec3 toCenter = pos - u_transformationCenter;
    float distanceFromCenter = length(toCenter);
    
    // Only affect vertices within transformation radius
    if (distanceFromCenter > u_transformationRadius) return pos;
    
    float influence = 1.0 - (distanceFromCenter / u_transformationRadius);
    influence = smoothstep(0.0, 1.0, influence);
    
    // Generate organic noise for transformation
    float noise1 = noise3D(pos * 2.0 + vec3(u_time * 0.1));
    float noise2 = noise3D(pos * 4.0 + vec3(u_time * 0.05, 100.0, 0.0));
    float noise3 = noise3D(pos * 8.0 + vec3(0.0, u_time * 0.03, 200.0));
    
    // Create organic, breathing transformation
    vec3 organicOffset = vec3(
        sin(u_time * 2.0 + pos.x * 5.0) * noise1,
        cos(u_time * 1.5 + pos.y * 4.0) * noise2,
        sin(u_time * 1.8 + pos.z * 3.0) * noise3
    ) * 0.3;
    
    // Apply transformation with phase and influence
    return pos + organicOffset * phase * influence;
}

vec3 ApplyCrystallineTransformation(vec3 pos, float phase) {
    if (phase <= 0.0) return pos;
    
    vec3 toCenter = pos - u_transformationCenter;
    float distanceFromCenter = length(toCenter);
    
    if (distanceFromCenter > u_transformationRadius) return pos;
    
    float influence = 1.0 - (distanceFromCenter / u_transformationRadius);
    influence = influence * influence; // Sharper falloff for crystalline effect
    
    // Create angular, geometric transformation
    vec3 crystallineDir = normalize(toCenter + vec3(sin(u_time), cos(u_time * 0.7), sin(u_time * 0.5)));
    float crystallinePush = sin(distanceFromCenter * 10.0 + u_time * 3.0) * 0.2;
    
    vec3 crystallineOffset = crystallineDir * crystallinePush;
    
    return pos + crystallineOffset * phase * influence;
}

vec3 ApplyDissolutionTransformation(vec3 pos, float phase) {
    if (phase <= 0.0) return pos;
    
    vec3 toCenter = pos - u_transformationCenter;
    float distanceFromCenter = length(toCenter);
    
    if (distanceFromCenter > u_transformationRadius) return pos;
    
    // Dissolution effect - vertices move away and become unstable
    float dissolveNoise = noise3D(pos * 10.0 + vec3(u_time));
    
    // Only dissolve if noise is above threshold based on phase
    float dissolveThreshold = 1.0 - phase;
    if (dissolveNoise < dissolveThreshold) {
        // This vertex is dissolved - push it away dramatically
        vec3 dissolveDir = normalize(toCenter + vec3(
            sin(u_time * 5.0 + pos.x * 10.0),
            cos(u_time * 4.0 + pos.y * 10.0),
            sin(u_time * 3.0 + pos.z * 10.0)
        ));
        
        float dissolveStrength = (dissolveThreshold - dissolveNoise) / dissolveThreshold;
        return pos + dissolveDir * dissolveStrength * 2.0;
    }
    
    return pos;
}

vec3 ApplyAncientDistortion(vec3 pos) {
    if (u_ancientDistortion <= 0.0) return pos;
    
    // Ancient, cosmic distortion that affects reality itself
    float cosmicNoise = noise3D(pos * 0.5 + vec3(u_time * 0.02));
    float ancientPulse = sin(u_time * 0.5 + length(pos) * 0.1) * 0.5 + 0.5;
    
    vec3 distortionField = vec3(
        sin(pos.y * 2.0 + u_time * 0.3) * cosmicNoise,
        cos(pos.z * 2.0 + u_time * 0.2) * ancientPulse,
        sin(pos.x * 2.0 + u_time * 0.4) * (cosmicNoise + ancientPulse) * 0.5
    );
    
    return pos + distortionField * u_ancientDistortion * 0.1;
}

void main() {
    vec3 worldPos = (u_model * vec4(aPos, 1.0)).xyz;
    
    // Apply ancient distortion first (affects everything subtly)
    worldPos = ApplyAncientDistortion(worldPos);
    
    // Apply transformation effects based on type
    if (u_morphingPhase > 0.0) {
        if (u_transformationType == 0) {
            worldPos = ApplyOrganicTransformation(worldPos, u_morphingPhase);
        } else if (u_transformationType == 1) {
            worldPos = ApplyCrystallineTransformation(worldPos, u_morphingPhase);
        } else if (u_transformationType == 2) {
            worldPos = ApplyDissolutionTransformation(worldPos, u_morphingPhase);
        }
    }
    
    gl_Position = u_projection * u_view * vec4(worldPos, 1.0);
    
    // Pass data to fragment shader
    WorldPos = worldPos;
    Normal = mat3(transpose(inverse(u_model))) * aNormal;
    TexCoord = aTexCoord;
    
    // Calculate transformation factor for fragment shader effects
    float distanceToCenter = length(worldPos - u_transformationCenter);
    TransformationFactor = 1.0 - clamp(distanceToCenter / u_transformationRadius, 0.0, 1.0);
    TransformationFactor *= u_morphingPhase;
}