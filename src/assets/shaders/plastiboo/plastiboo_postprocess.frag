#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_sceneTexture;
uniform sampler2D u_depthTexture;
uniform sampler3D u_organicNoise;
uniform float u_time;
uniform vec2 u_resolution;

// Plastiboo effect parameters
uniform float u_vertexSnapEffect = 0.7;      // PS1 pixelation strength
uniform float u_paletteReduction = 0.8;      // Color quantization
uniform float u_organicBreathing = 0.3;      // Living, breathing effect
uniform float u_horrorAtmosphere = 0.5;      // Unsettling distortions
uniform float u_ancientDistortion = 0.2;     // Mysterious warping
uniform int u_paletteType = 0;               // Which palette to use

// Enhanced Plastiboo palettes
const vec3 medievalPalette[8] = vec3[](
    vec3(0.08, 0.06, 0.04),   // Deep shadow
    vec3(0.15, 0.12, 0.08),   // Dark stone
    vec3(0.25, 0.20, 0.12),   // Worn stone  
    vec3(0.40, 0.32, 0.20),   // Old wood
    vec3(0.55, 0.45, 0.25),   // Torch light
    vec3(0.70, 0.60, 0.35),   // Warm highlight
    vec3(0.12, 0.15, 0.08),   // Moss shadow
    vec3(0.35, 0.40, 0.25)    // Forest green
);

const vec3 ancientForestPalette[8] = vec3[](
    vec3(0.05, 0.08, 0.05),   // Deep forest shadow
    vec3(0.12, 0.18, 0.10),   // Dark moss
    vec3(0.20, 0.28, 0.15),   // Forest green
    vec3(0.35, 0.25, 0.15),   // Tree bark
    vec3(0.45, 0.35, 0.20),   // Autumn light
    vec3(0.25, 0.35, 0.20),   // Sage green
    vec3(0.15, 0.25, 0.12),   // Deep green
    vec3(0.40, 0.45, 0.25)    // Bright moss
);

const vec3 cursedMonasteryPalette[8] = vec3[](
    vec3(0.08, 0.06, 0.12),   // Purple shadow
    vec3(0.15, 0.12, 0.18),   // Cold stone
    vec3(0.22, 0.20, 0.25),   // Ghostly gray
    vec3(0.35, 0.30, 0.40),   // Pale purple
    vec3(0.50, 0.45, 0.55),   // Ethereal light
    vec3(0.18, 0.15, 0.25),   // Dark violet
    vec3(0.30, 0.25, 0.35),   // Mystic purple
    vec3(0.60, 0.55, 0.65)    // Ghost white
);

const vec3 plagueVillagePalette[8] = vec3[](
    vec3(0.12, 0.10, 0.06),   // Diseased shadow
    vec3(0.22, 0.18, 0.08),   // Sickly yellow
    vec3(0.30, 0.25, 0.12),   // Rotting wood
    vec3(0.18, 0.22, 0.08),   // Plague green
    vec3(0.35, 0.30, 0.15),   // Feverish light
    vec3(0.25, 0.20, 0.10),   // Decay brown
    vec3(0.28, 0.24, 0.10),   // Sick yellow-green
    vec3(0.40, 0.35, 0.18)    // Pale sick light
);

vec3 GetCurrentPalette(int index) {
    if (u_paletteType == 1) {
        return ancientForestPalette[index];
    } else if (u_paletteType == 2) {
        return cursedMonasteryPalette[index];
    } else if (u_paletteType == 3) {
        return plagueVillagePalette[index];
    } else {
        return medievalPalette[index];  // Default
    }
}

vec3 QuantizeToPlastibooPalette(vec3 color) {
    float minDistance = 1000.0;
    vec3 nearestColor = GetCurrentPalette(0);
    
    for (int i = 0; i < 8; ++i) {
        vec3 paletteColor = GetCurrentPalette(i);
        float distance = length(color - paletteColor);
        if (distance < minDistance) {
            minDistance = distance;
            nearestColor = paletteColor;
        }
    }
    
    return nearestColor;
}

vec3 ApplyOrganicBreathing(vec3 color, vec2 screenPos) {
    if (u_organicBreathing <= 0.0) return color;
    
    // Create living, breathing effect that makes surfaces feel alive
    float breathingPattern = sin(u_time * 2.0 + screenPos.x * 5.0) * 
                            cos(u_time * 1.5 + screenPos.y * 3.0);
    
    // Sample organic noise for detail variation
    vec3 noiseCoords = vec3(screenPos * 2.0, u_time * 0.1);
    float organicNoise = texture(u_organicNoise, noiseCoords * 0.1).r;
    
    // Combine breathing with organic noise
    float breathingIntensity = (breathingPattern * 0.5 + 0.5) * organicNoise;
    breathingIntensity *= u_organicBreathing;
    
    // Apply subtle color shift that makes everything feel alive
    color += vec3(breathingIntensity * 0.05, breathingIntensity * 0.03, breathingIntensity * 0.02);
    
    return color;
}

vec3 ApplyHorrorAtmosphere(vec3 color, vec2 screenPos, float depth) {
    if (u_horrorAtmosphere <= 0.0) return color;
    
    // Add unsettling screen distortions
    vec2 distortedCoords = screenPos + vec2(
        sin(u_time * 3.0 + screenPos.y * 15.0) * 0.003,
        cos(u_time * 2.5 + screenPos.x * 12.0) * 0.002
    ) * u_horrorAtmosphere;
    
    // Depth-based fog with eerie color shift
    float fogFactor = 1.0 - exp(-depth * 0.08);
    vec3 fogColor = GetCurrentPalette(0) * 0.7; // Use darkest palette color for fog
    color = mix(color, fogColor, fogFactor * u_horrorAtmosphere);
    
    // Vignetting for claustrophobic feel
    vec2 vignetteCoord = (screenPos - 0.5) * 2.0;
    float vignette = 1.0 - length(vignetteCoord) * 0.4 * u_horrorAtmosphere;
    color *= vignette;
    
    return color;
}

vec3 ApplyAncientDistortion(vec3 color, vec2 screenPos) {
    if (u_ancientDistortion <= 0.0) return color;
    
    // Create mysterious, ancient warping effects
    float distortionPattern = sin(u_time + screenPos.x * 8.0) * 
                             cos(u_time * 0.7 + screenPos.y * 6.0);
    
    // Add subtle color shifts that suggest transformation
    float hueShift = distortionPattern * u_ancientDistortion * 0.1;
    
    // Simple hue shifting (approximate)
    float avg = (color.r + color.g + color.b) / 3.0;
    color.r = mix(color.r, avg + sin(hueShift) * 0.1, abs(hueShift));
    color.g = mix(color.g, avg + sin(hueShift + 2.094) * 0.1, abs(hueShift));
    color.b = mix(color.b, avg + sin(hueShift + 4.188) * 0.1, abs(hueShift));
    
    return color;
}

vec3 ApplyPS1VertexSnapping(vec3 color, vec2 screenPos) {
    if (u_vertexSnapEffect <= 0.0) return color;
    
    // Simulate PS1-style vertex snapping by creating subtle grid artifacts
    vec2 snappedCoords = floor(screenPos * u_resolution * u_vertexSnapEffect) / 
                         (u_resolution * u_vertexSnapEffect);
    
    // Create subtle color banding that mimics vertex snapping artifacts
    float snapArtifact = length(screenPos - snappedCoords) * 10.0;
    snapArtifact = fract(snapArtifact);
    
    // Apply subtle color shift based on snapping
    color *= 1.0 - snapArtifact * u_vertexSnapEffect * 0.05;
    
    return color;
}

void main() {
    vec2 screenPos = gl_FragCoord.xy / u_resolution;
    
    // Sample the original scene
    vec4 originalColor = texture(u_sceneTexture, TexCoord);
    float depth = texture(u_depthTexture, TexCoord).r;
    
    vec3 finalColor = originalColor.rgb;
    
    // Apply PS1-style vertex snapping artifacts
    finalColor = ApplyPS1VertexSnapping(finalColor, screenPos);
    
    // Apply organic breathing effect
    finalColor = ApplyOrganicBreathing(finalColor, screenPos);
    
    // Apply horror atmosphere effects
    finalColor = ApplyHorrorAtmosphere(finalColor, screenPos, depth);
    
    // Apply ancient distortion effects
    finalColor = ApplyAncientDistortion(finalColor, screenPos);
    
    // Apply palette reduction for authentic Plastiboo look
    if (u_paletteReduction > 0.0) {
        vec3 quantized = QuantizeToPlastibooPalette(finalColor);
        finalColor = mix(finalColor, quantized, u_paletteReduction);
    }
    
    // Add subtle film grain
    vec3 noiseCoords = vec3(screenPos * 100.0, u_time);
    float grain = texture(u_organicNoise, noiseCoords * 0.01).r;
    finalColor += (grain - 0.5) * 0.03;
    
    FragColor = vec4(finalColor, originalColor.a);
}