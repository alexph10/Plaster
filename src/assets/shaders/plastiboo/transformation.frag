#version 460 core

in vec2 TexCoord;
in vec3 WorldPos;
in vec3 Normal;
in float TransformationFactor;

out vec4 FragColor;

uniform sampler2D u_baseTexture;
uniform sampler2D u_transformationTexture;  // Special texture for transformed areas
uniform sampler2D u_noiseTexture;
uniform float u_time;

// Transformation parameters
uniform float u_morphingPhase = 0.0;
uniform int u_transformationType = 0;     // 0=organic, 1=crystalline, 2=dissolution
uniform vec3 u_transformationColor = vec3(0.3, 0.7, 0.4);  // Color shift during transformation

// Lighting for transformed areas
uniform vec3 u_transformationGlow = vec3(0.2, 0.5, 0.3);

// Plastiboo palette for color quantization
const vec3 transformationPalette[6] = vec3[](
    vec3(0.1, 0.15, 0.1),    // Dark transformation
    vec3(0.2, 0.3, 0.2),     // Medium green
    vec3(0.3, 0.5, 0.35),    // Bright transformation
    vec3(0.15, 0.25, 0.4),   // Mystic blue
    vec3(0.4, 0.3, 0.2),     // Earth tone
    vec3(0.5, 0.6, 0.4)      // Highlight
);

vec3 QuantizeTransformationColor(vec3 color) {
    float minDistance = 1000.0;
    vec3 nearestColor = transformationPalette[0];
    
    for (int i = 0; i < 6; ++i) {
        float distance = length(color - transformationPalette[i]);
        if (distance < minDistance) {
            minDistance = distance;
            nearestColor = transformationPalette[i];
        }
    }
    
    return nearestColor;
}

vec3 ApplyOrganicTransformationEffect(vec3 baseColor) {
    // Organic transformation creates living, breathing color effects
    float organicPulse = sin(u_time * 3.0 + WorldPos.x * 5.0) * 
                        cos(u_time * 2.0 + WorldPos.z * 4.0) * 0.5 + 0.5;
    
    // Sample transformation texture with animated coordinates
    vec2 organicTexCoord = TexCoord + vec2(
        sin(u_time * 1.5 + WorldPos.y * 3.0) * 0.05,
        cos(u_time * 1.2 + WorldPos.x * 3.0) * 0.05
    ) * TransformationFactor;
    
    vec3 organicTexture = texture(u_transformationTexture, organicTexCoord).rgb;
    
    // Mix base color with organic transformation
    vec3 organicColor = mix(baseColor, baseColor * organicTexture * u_transformationColor, 
                           TransformationFactor * organicPulse);
    
    // Add organic glow
    organicColor += u_transformationGlow * TransformationFactor * organicPulse;
    
    return organicColor;
}

vec3 ApplyCrystallineTransformationEffect(vec3 baseColor) {
    // Crystalline transformation creates sharp, angular color shifts
    float crystallinePattern = step(0.5, sin(WorldPos.x * 10.0 + u_time) * 
                                         cos(WorldPos.z * 8.0 + u_time * 0.7));
    
    // Create sharp color transitions
    vec3 crystallineColor = mix(baseColor, u_transformationColor, 
                               TransformationFactor * crystallinePattern);
    
    // Add sharp highlights
    float highlight = step(0.8, sin(WorldPos.y * 15.0 + u_time * 2.0));
    crystallineColor += vec3(0.3, 0.6, 0.8) * highlight * TransformationFactor;
    
    return crystallineColor;
}

vec3 ApplyDissolutionTransformationEffect(vec3 baseColor) {
    // Dissolution effect makes pixels disappear or become unstable
    float dissolveNoise = texture(u_noiseTexture, TexCoord * 5.0 + vec2(u_time * 0.1)).r;
    
    float dissolveThreshold = 1.0 - TransformationFactor;
    if (dissolveNoise < dissolveThreshold) {
        // Pixel is dissolving - create static/noise effect
        vec3 staticNoise = vec3(
            fract(sin(dot(TexCoord + u_time, vec2(12.9898, 78.233))) * 43758.5453),
            fract(sin(dot(TexCoord + u_time + 0.1, vec2(15.9898, 85.233))) * 47358.5453),
            fract(sin(dot(TexCoord + u_time + 0.2, vec2(18.9898, 92.233))) * 51358.5453)
        );
        
        return mix(baseColor, staticNoise, (dissolveThreshold - dissolveNoise) / dissolveThreshold);
    }
    
    // Pixel is not dissolved but might be affected
    return mix(baseColor, baseColor * 0.7, TransformationFactor * 0.3);
}

void main() {
    vec4 baseColor = texture(u_baseTexture, TexCoord);
    vec3 finalColor = baseColor.rgb;
    
    // Apply transformation effects based on transformation factor
    if (TransformationFactor > 0.0) {
        if (u_transformationType == 0) {
            finalColor = ApplyOrganicTransformationEffect(finalColor);
        } else if (u_transformationType == 1) {
            finalColor = ApplyCrystallineTransformationEffect(finalColor);
        } else if (u_transformationType == 2) {
            finalColor = ApplyDissolutionTransformationEffect(finalColor);
        }
        
        // Quantize transformed colors to maintain Plastiboo aesthetic
        if (TransformationFactor > 0.3) {
            vec3 quantized = QuantizeTransformationColor(finalColor);
            finalColor = mix(finalColor, quantized, TransformationFactor * 0.7);
        }
    }
    
    // Add subtle edge effects for transformation areas
    if (TransformationFactor > 0.1) {
        float edgeEffect = smoothstep(0.1, 0.3, TransformationFactor) * 
                          smoothstep(0.9, 0.7, TransformationFactor);
        finalColor += u_transformationGlow * edgeEffect * 0.5;
    }
    
    FragColor = vec4(finalColor, baseColor.a);
}