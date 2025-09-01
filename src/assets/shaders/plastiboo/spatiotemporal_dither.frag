#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_sceneTexture;
uniform sampler3D u_spatiotemporalNoise;  // 128x128x64 sequence
uniform int u_frameIndex;
uniform int u_colorLevels;
uniform float u_ditherStrength;

const vec3 medievalPalette[6] = vec3[](
    vec3(0.1, 0.08, 0.05),  // Dark brown (shadows)
    vec3(0.3, 0.25, 0.15),  // Medium brown (stone)
    vec3(0.5, 0.4, 0.25),   // Light brown (wood)
    vec3(0.4, 0.3, 0.2),    // Rust brown (metal)
    vec3(0.2, 0.15, 0.1),   // Deep shadow
    vec3(0.7, 0.6, 0.4)     // Highlight (torch light)
);

vec3 FindNearestPaletteColor(vec3 color) {
    float minDistance = 1000.0;
    vec3 nearestColor = medievalPalette[0];
    
    for (int i = 0; i < 6; ++i) {
        float distance = length(color - medievalPalette[i]);
        if (distance < minDistance) {
            minDistance = distance;
            nearestColor = medievalPalette[i];
        }
    }
    
    return nearestColor;
}

void main() {
    vec4 originalColor = texture(u_sceneTexture, TexCoord);
    
    vec3 noiseCoords = vec3(
        mod(gl_FragCoord.xy, 128.0) / 128.0,
        float(u_frameIndex % 64) / 64.0
    );
    float noiseValue = texture(u_spatiotemporalNoise, noiseCoords).r;
    
    vec3 ditheredColor = originalColor.rgb + (noiseValue - 0.5) * u_ditherStrength * 0.1;
    
    vec3 quantizedColor = FindNearestPaletteColor(ditheredColor);
    
    FragColor = vec4(quantizedColor, originalColor.a);
}

