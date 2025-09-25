#version 460 core

layout(location = 0) out vec4 FragColor;

uniform float u_time;
uniform vec2 u_resolution;
uniform vec3 u_noiseParams; // x: scale, y: octaves, z: persistence

// 3D Noise function for organic patterns
float hash(vec3 p) {
    p = fract(p * vec3(443.8975, 397.2973, 491.1871));
    p += dot(p, p.yxz + 19.19);
    return fract((p.x + p.y) * p.z);
}

float noise3D(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); // Smooth interpolation
    
    return mix(mix(mix(hash(i + vec3(0, 0, 0)), 
                       hash(i + vec3(1, 0, 0)), f.x),
                   mix(hash(i + vec3(0, 1, 0)), 
                       hash(i + vec3(1, 1, 0)), f.x), f.y),
               mix(mix(hash(i + vec3(0, 0, 1)), 
                       hash(i + vec3(1, 0, 1)), f.x),
                   mix(hash(i + vec3(0, 1, 1)), 
                       hash(i + vec3(1, 1, 1)), f.x), f.y), f.z);
}

float fbm(vec3 p, int octaves, float persistence) {
    float total = 0.0;
    float frequency = 1.0;
    float amplitude = 1.0;
    float maxValue = 0.0;
    
    for (int i = 0; i < octaves; i++) {
        total += noise3D(p * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }
    
    return total / maxValue;
}

// Generate organic, living patterns
vec4 GenerateOrganicNoise(vec2 coord, float time) {
    vec3 p = vec3(coord * u_noiseParams.x, time * 0.1);
    
    // Multiple noise layers for complex organic patterns
    float noise1 = fbm(p, int(u_noiseParams.y), u_noiseParams.z);
    float noise2 = fbm(p * 2.0 + vec3(100.0, 50.0, time * 0.05), int(u_noiseParams.y), u_noiseParams.z);
    float noise3 = fbm(p * 4.0 + vec3(200.0, 150.0, time * 0.02), int(u_noiseParams.y), u_noiseParams.z);
    
    // Create breathing, pulsing patterns
    float breathingPattern = sin(time * 2.0 + coord.x * 3.0) * cos(time * 1.5 + coord.y * 2.0) * 0.5 + 0.5;
    
    // Combine noises with breathing effect
    float organicDetail = noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2;
    organicDetail = mix(organicDetail, organicDetail * breathingPattern, 0.3);
    
    // Create color variation for different organic effects
    vec3 organicColor = vec3(
        organicDetail,
        noise2 * breathingPattern,
        noise3 * (1.0 - breathingPattern * 0.5)
    );
    
    return vec4(organicColor, organicDetail);
}

void main() {
    vec2 coord = gl_FragCoord.xy / u_resolution;
    
    FragColor = GenerateOrganicNoise(coord, u_time);
}