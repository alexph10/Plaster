#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_sceneTexture;
uniform sampler3D u_spatiotemporalNoise;  // 128x128x64 sequence
uniform sampler2D u_depthTexture;
uniform int u_frameIndex;
uniform int u_colorLevels;
uniform float u_ditherStrength;
uniform vec2 u_resolution;
uniform float u_time;

// PS1-style rendering artifacts
uniform float u_textureWarping = 0.3;      // Affine texture mapping artifacts
uniform float u_zBufferNoise = 0.02;       // Z-buffer precision issues
uniform float u_colorBanding = 0.5;        // Reduce color precision
uniform float u_scanlineIntensity = 0.3;   // CRT scanlines
uniform bool u_enablePS1Artifacts = true;

// Enhanced Plastiboo palettes with more colors for richer scenes
const vec3 medievalPalette[12] = vec3[](
    vec3(0.08, 0.06, 0.04),   // Deep shadow
    vec3(0.15, 0.12, 0.08),   // Dark brown
    vec3(0.25, 0.20, 0.12),   // Medium brown
    vec3(0.40, 0.32, 0.20),   // Light brown
    vec3(0.55, 0.45, 0.25),   // Warm wood
    vec3(0.70, 0.60, 0.35),   // Torch light
    vec3(0.12, 0.15, 0.08),   // Dark moss
    vec3(0.20, 0.25, 0.15),   // Forest green
    vec3(0.35, 0.40, 0.25),   // Sage green
    vec3(0.45, 0.35, 0.15),   // Rust/copper
    vec3(0.25, 0.20, 0.25),   // Purple shadow
    vec3(0.85, 0.75, 0.50)    // Ancient gold highlight
);

vec3 FindNearestPaletteColor(vec3 color) {
    float minDistance = 1000.0;
    vec3 nearestColor = medievalPalette[0];
    
    for (int i = 0; i < 12; ++i) {
        float distance = length(color - medievalPalette[i]);
        if (distance < minDistance) {
            minDistance = distance;
            nearestColor = medievalPalette[i];
        }
    }
    
    return nearestColor;
}

vec3 ApplyTextureWarping(vec2 texCoord, vec3 color) {
    if (!u_enablePS1Artifacts || u_textureWarping <= 0.0) return color;
    
    // Simulate affine texture mapping artifacts
    float depth = texture(u_depthTexture, texCoord).r;
    
    // Create texture warping based on depth (PS1 had no perspective correction)
    vec2 warpedCoord = texCoord + vec2(
        sin(texCoord.y * 20.0 + depth * 10.0) * u_textureWarping * 0.01,
        cos(texCoord.x * 15.0 + depth * 8.0) * u_textureWarping * 0.01
    );
    
    // Sample with warped coordinates
    vec3 warpedColor = texture(u_sceneTexture, warpedCoord).rgb;
    
    return mix(color, warpedColor, u_textureWarping * 0.3);
}

vec3 ApplyZBufferNoise(vec3 color, vec2 screenPos) {
    if (!u_enablePS1Artifacts || u_zBufferNoise <= 0.0) return color;
    
    // Simulate Z-buffer precision issues that cause pixel flickering
    float depth = texture(u_depthTexture, TexCoord).r;
    
    // Create noise based on screen position and depth
    float zNoise = fract(sin(dot(screenPos + depth, vec2(12.9898, 78.233))) * 43758.5453);
    zNoise = (zNoise - 0.5) * 2.0; // -1 to 1 range
    
    // Apply Z-buffer noise that's more visible at distance
    float noiseIntensity = depth * u_zBufferNoise;
    color += vec3(zNoise * noiseIntensity);
    
    return color;
}

vec3 ApplyColorBanding(vec3 color) {
    if (u_colorBanding <= 0.0) return color;
    
    // Reduce color precision to simulate low bit-depth
    float colorLevels = mix(256.0, 32.0, u_colorBanding);
    
    vec3 bandedColor = vec3(
        floor(color.r * colorLevels) / colorLevels,
        floor(color.g * colorLevels) / colorLevels,
        floor(color.b * colorLevels) / colorLevels
    );
    
    return mix(color, bandedColor, u_colorBanding);
}

vec3 ApplyScanlines(vec3 color, vec2 screenPos) {
    if (u_scanlineIntensity <= 0.0) return color;
    
    // Create CRT-style scanlines
    float scanline = sin(screenPos.y * u_resolution.y * 2.0) * 0.5 + 0.5;
    scanline = mix(1.0, scanline, u_scanlineIntensity);
    
    // Add phosphor persistence effect
    float phosphor = sin(screenPos.y * u_resolution.y * 6.0 + u_time * 10.0) * 0.1 + 0.9;
    
    color *= scanline * phosphor;
    
    return color;
}

vec3 ApplyRetroFilmGrain(vec3 color, vec2 screenPos) {
    // Generate film grain noise
    float grain = fract(sin(dot(screenPos + u_time, vec2(12.9898, 78.233))) * 43758.5453);
    grain = (grain - 0.5) * 0.1; // Reduce intensity
    
    // Add temporal variation
    float temporalGrain = sin(u_time * 50.0 + screenPos.x * 100.0) * 0.02;
    
    color += vec3(grain + temporalGrain);
    
    return color;
}

void main() {
    vec2 screenPos = gl_FragCoord.xy / u_resolution;
    vec4 originalColor = texture(u_sceneTexture, TexCoord);
    vec3 finalColor = originalColor.rgb;
    
    // Apply PS1-style texture warping
    finalColor = ApplyTextureWarping(TexCoord, finalColor);
    
    // Apply Z-buffer noise artifacts
    finalColor = ApplyZBufferNoise(finalColor, screenPos);
    
    // Apply color banding
    finalColor = ApplyColorBanding(finalColor);
    
    // Original spatiotemporal dithering with enhancements
    vec3 noiseCoords = vec3(
        mod(gl_FragCoord.xy, 128.0) / 128.0,
        float(u_frameIndex % 64) / 64.0
    );
    float noiseValue = texture(u_spatiotemporalNoise, noiseCoords).r;
    
    // Enhanced dithering with depth consideration
    float depth = texture(u_depthTexture, TexCoord).r;
    float ditherIntensity = u_ditherStrength * (1.0 + depth * 0.5); // More dither at distance
    
    vec3 ditheredColor = finalColor + (noiseValue - 0.5) * ditherIntensity * 0.1;
    
    // Quantize to Plastiboo palette
    vec3 quantizedColor = FindNearestPaletteColor(ditheredColor);
    
    // Apply CRT scanlines
    quantizedColor = ApplyScanlines(quantizedColor, screenPos);
    
    // Add retro film grain
    quantizedColor = ApplyRetroFilmGrain(quantizedColor, screenPos);
    
    // Add subtle color breathing effect
    float breathe = sin(u_time * 1.5 + screenPos.x + screenPos.y) * 0.01 + 1.0;
    quantizedColor *= breathe;
    
    FragColor = vec4(quantizedColor, originalColor.a);
}

