#version 450 

layout(location = 0) in vec3 fragWorldPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragColor;
layout(location = 4) in vec3 fragVertexLighting;
layout(location = 5) in float fragDepth;


layout(location = 0) out vec4 outColor;


layout(set = 2, binding = 0) uniform MaterialUBO {
  vec4 baseColor;
  float clayRoughness;
  float ditherStrength;
  float warmthBias;
  int paletteIndex;
  int useDithering;
  int useAffineMapping;
} material;



layout(set = 2, binding = 1) uniform sampler2D paletteTex;

layout(set = 2, binding = 2) uniform sampler2D blueNoiseTex;

layout(set = 2, binding = 3) uniform sampler2D albedoTex;


const int bayerMatrix[64] = int[](
  0, 32, 8, 40, 2, 34, 10, 42,
  48, 16, 56, 24, 50, 18, 58, 26,
  12, 44, 4, 36, 14, 16, 6, 38,
  60, 28, 52, 20, 62, 30, 54, 22,
  3, 35, 11, 43, 1, 33, 9, 41,
  51, 19, 59, 27, 49, 17, 57, 25,
  15, 47, 7, 39, 13, 45, 5, 37,
  63, 31, 55, 23, 61, 29, 53, 21
);

float getBayerThreshold() {
  ivec2 pixelPos = ivec2(gl_FragCoord.xy);
  int x = pixelPos.x % 8;
  int y = pixelPos.y % 8;
  int index = y * 8 + x;
  return float(bayerMatrix[index]) / 64.0;
}

vec3 rgb2hsv(vec3 c) {
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
  vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

  float d = q.x - min(q.w, q.y);
  float e = 1.0e-10;
  return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


vec3 applyWarmBias(vec3 color, float bias) {
  vec3 hsv = rgb2hsv(color);
  
  float targetHue = 0.083;
  hsv.x = mix(hsv.x, targetHue, bias * 0.3);
  
  hsv.y = mix(hsv.y, min(hsv.y * 1.2, 1.0), bias);

  return hsv2rgb(hsv);
}

vec3 quantizeColor(vec3 color, int bitsPerChannel) {
  float levels = pow(2.0, float(bitsPerChannel));
  return floor(color * levels) / levels;
}

vec3 quantizeToPalette(vec3 color) {
  return quantizeColor(color, 5);
}

vec3 applyDithering(vec3 color, float strength) {
  float threshold = getBayerThreshold();

  vec3 dither = vec3(threshold - 0.5) * strength * 0.05;
  return color + dither;
}

void main() {
  vec2 texCoord = fragTexCoord;
  if (material.useAffineMapping == 1) {
    texCoord = fragTexCoord / fragDepth;
  }

  vec3 albedo = material.baseColor.rgb * fragColor;

  // albedo *= texture(albedoTex, texCoord).rgb;

  vec3 litColor = albedo * fragVertexLighting;
  
  litColor = applyWarmBias(litColor, material.warmthBias);

  if (material.useDithering == 1) {
    litColor = applyDithering(litColor, material.ditherStrength);
  }

  vec3 finalColor = quantizeToPalette(litColor);
  
  finalColor = clamp(finalColor, 0.0, 1.0);
  
  outColor = vec4(finalColor, 1.0);
}


