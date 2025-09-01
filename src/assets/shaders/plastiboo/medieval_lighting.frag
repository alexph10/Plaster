#version 460 core

in vec2 TexCoord;
in vec3 WorldPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D u_baseTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_grungeTexture;

uniform vec3 u_candleLights[8];        // Medieval light positions
uniform vec3 u_candleColors[8];        // Warm, flickering colors
uniform float u_candleIntensities[8];  // Dynamic intensities
uniform int u_numLights;

uniform float u_ambientOcclusion;
uniform float u_shadowHardness;
uniform float u_grungeIntensity;

vec3 CalculateMedievalLighting(vec3 worldPos, vec3 normal, vec3 baseColor) {
    vec3 totalLight = vec3(0.1, 0.08, 0.05); // Very dark ambient
    
    for (int i = 0; i < u_numLights && i < 8; ++i) {
        vec3 lightDir = u_candleLights[i] - worldPos;
        float distance = length(lightDir);
        lightDir = normalize(lightDir);
        
        float attenuation = 1.0 / (1.0 + 0.5 * distance + 0.3 * distance * distance);
        
        float ndotl = max(0.0, dot(normal, lightDir));
        ndotl = pow(ndotl, u_shadowHardness);

        float flicker = 0.9 + 0.1 * sin(u_candleIntensities[i] * 10.0);
        
        totalLight += u_candleColors[i] * ndotl * attenuation * flicker;
    }
    
    return baseColor * totalLight;
}

void main() {
    vec4 baseColor = texture(u_baseTexture, TexCoord);
    vec3 normal = normalize(texture(u_normalTexture, TexCoord).rgb * 2.0 - 1.0);
    

    vec3 litColor = CalculateMedievalLighting(WorldPos, normal, baseColor.rgb);
    
    vec4 grunge = texture(u_grungeTexture, TexCoord * 2.0);
    litColor = mix(litColor, litColor * grunge.rgb, u_grungeIntensity);
    
    float cornerDarkening = 1.0 - u_ambientOcclusion * 
                           (1.0 - smoothstep(0.0, 0.5, min(TexCoord.x, 1.0 - TexCoord.x))) *
                           (1.0 - smoothstep(0.0, 0.5, min(TexCoord.y, 1.0 - TexCoord.y)));
    
    litColor *= cornerDarkening;
    
    FragColor = vec4(litColor, baseColor.a);
}

