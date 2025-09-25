#version 460 core

in vec2 TexCoord;
in vec3 WorldPos;
in vec3 Normal;
in vec3 VertexColor;
in float VertexDepth;

out vec4 FragColor;

uniform sampler2D u_baseTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_detailTexture;      // For organic details
uniform sampler2D u_noiseTexture;       // For procedural patterns
uniform sampler3D u_organicNoise;       // 3D noise for living effects

uniform float u_time;
uniform vec2 u_resolution;

// Plastiboo-specific lighting
uniform vec3 u_plastibooLights[16];
uniform vec3 u_plastibooLightColors[16];
uniform float u_plastibooLightIntensities[16];
uniform int u_numPlastibooLights;

// Material properties for Plastiboo aesthetic
uniform float u_organicDetailStrength = 0.7;
uniform float u_breathingIntensity = 0.3;
uniform float u_ancientGlow = 0.2;
uniform float u_horrorAtmosphere = 0.5;
uniform vec3 u_paletteShift = vec3(1.0);  // RGB multipliers for palette variation

// Enhanced Plastiboo palette - more colors for richer scenes
const vec3 plastibooBasePalette[12] = vec3[](
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

vec3 FindNearestPlastibooPaletteColor(vec3 color) {
    // Apply palette shift for variation
    color *= u_paletteShift;
    
    float minDistance = 1000.0;
    vec3 nearestColor = plastibooBasePalette[0];
    
    for (int i = 0; i < 12; ++i) {
        float distance = length(color - plastibooBasePalette[i]);
        if (distance < minDistance) {
            minDistance = distance;
            nearestColor = plastibooBasePalette[i];
        }
    }
    
    return nearestColor;
}

vec3 CalculateOrganicDetails(vec2 texCoord, vec3 worldPos) {
    // Create the "breathing", alive quality of Plastiboo's work
    float time = u_time * 0.5;
    
    // Multiple layers of organic noise
    float noise1 = texture(u_organicNoise, vec3(worldPos * 0.1 + time * 0.1)).r;
    float noise2 = texture(u_organicNoise, vec3(worldPos * 0.3 + time * 0.05)).g;
    float noise3 = texture(u_organicNoise, vec3(worldPos * 0.7 + time * 0.02)).b;
    
    // Breathing pattern - makes surfaces feel alive
    float breathe = sin(time * 2.0 + worldPos.x * 3.0) * 
                   cos(time * 1.5 + worldPos.z * 3.0) * 0.5 + 0.5;
    
    // Combine noises for organic detail
    float organicMask = (noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2) * breathe;
    
    // Create detail texture coordinates that shift subtly
    vec2 detailCoord = texCoord * 4.0 + organicMask * 0.1;
    vec3 detailColor = texture(u_detailTexture, detailCoord).rgb;
    
    return mix(vec3(1.0), detailColor, u_organicDetailStrength * organicMask);
}

vec3 CalculatePlastibooLighting(vec3 worldPos, vec3 normal, vec3 baseColor) {
    // Enhanced lighting for atmospheric horror
    vec3 totalLight = vec3(0.05, 0.04, 0.03) * u_horrorAtmosphere;  // Very dark base
    
    for (int i = 0; i < u_numPlastibooLights && i < 16; ++i) {
        vec3 lightDir = u_plastibooLights[i] - worldPos;
        float distance = length(lightDir);
        lightDir = normalize(lightDir);
        
        // Non-linear attenuation for dramatic lighting
        float attenuation = 1.0 / (1.0 + 0.3 * distance + 0.1 * distance * distance);
        attenuation = pow(attenuation, 1.5);  // More dramatic falloff
        
        float ndotl = max(0.0, dot(normal, lightDir));
        
        // Add mysterious flickering
        float flicker = 0.8 + 0.2 * sin(u_time * 5.0 + u_plastibooLightIntensities[i] * 10.0);
        flicker *= 0.9 + 0.1 * sin(u_time * 3.0 + distance * 2.0);
        
        totalLight += u_plastibooLightColors[i] * ndotl * attenuation * flicker;
    }
    
    // Add ancient, mysterious glow
    if (u_ancientGlow > 0.0) {
        float glowPattern = sin(worldPos.x * 2.0 + u_time) * 
                           cos(worldPos.z * 2.0 + u_time * 0.7) * 0.5 + 0.5;
        totalLight += vec3(0.1, 0.07, 0.05) * u_ancientGlow * glowPattern;
    }
    
    return baseColor * totalLight;
}

vec3 ApplyHorrorAtmosphere(vec3 color, vec2 screenCoord, float depth) {
    // Add unsettling atmospheric effects
    
    // Depth-based fog with color shift
    float fogFactor = 1.0 - exp(-depth * 0.1);
    vec3 fogColor = vec3(0.08, 0.06, 0.05) * u_horrorAtmosphere;
    color = mix(color, fogColor, fogFactor);
    
    // Subtle screen distortion
    vec2 distortion = vec2(
        sin(u_time * 2.0 + screenCoord.y * 20.0) * 0.002,
        cos(u_time * 1.5 + screenCoord.x * 15.0) * 0.002
    ) * u_horrorAtmosphere;
    
    // Apply vignetting for claustrophobic feel
    vec2 vignetteCoord = (screenCoord - 0.5) * 2.0;
    float vignette = 1.0 - length(vignetteCoord) * 0.3 * u_horrorAtmosphere;
    color *= vignette;
    
    return color;
}

void main() {
    vec2 screenCoord = gl_FragCoord.xy / u_resolution;
    
    // Sample base texture with PS1-style filtering
    vec4 baseColor = texture(u_baseTexture, TexCoord);
    
    // Apply organic details that make surfaces feel alive
    vec3 organicDetails = CalculateOrganicDetails(TexCoord, WorldPos);
    baseColor.rgb *= organicDetails;
    
    // Mix with vertex colors for additional variation
    baseColor.rgb = mix(baseColor.rgb, VertexColor, 0.3);
    
    // Calculate Plastiboo-style lighting
    vec3 litColor = CalculatePlastibooLighting(WorldPos, normalize(Normal), baseColor.rgb);
    
    // Apply horror atmosphere effects
    litColor = ApplyHorrorAtmosphere(litColor, screenCoord, abs(VertexDepth));
    
    // Quantize to Plastiboo palette for authentic look
    litColor = FindNearestPlastibooPaletteColor(litColor);
    
    // Add subtle breathing effect to final color
    float breathe = sin(u_time * 1.5 + WorldPos.x + WorldPos.z) * 0.02 + 1.0;
    litColor *= breathe * u_breathingIntensity + (1.0 - u_breathingIntensity);
    
    FragColor = vec4(litColor, baseColor.a);
}