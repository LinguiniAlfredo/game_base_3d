#version 330 core

uniform sampler2D uScene;   // your rendered scene
uniform vec2 uResolution;   // output resolution
uniform float uTime;        // optional for noise flicker

in vec2 vUV;
out vec4 FragColor;

// --- Tweakable parameters ---
const float curvature = 0.20;         // screen curvature
const float scanline_strength = 0.25; // scanline darkness
const float mask_strength = 0.20;     // shadow mask effect
const float vignette_strength = 0.35; // dark edges
const float soften_amount = 0.30;     // blur/soften factor

// Apply simple barrel distortion
vec2 barrelDistortion(vec2 uv) {
    vec2 cc = uv - 0.5;
    float dist = dot(cc, cc);
    uv = uv + cc * dist * curvature;
    return uv;
}

// Simple shadow mask (RGB stripe)
vec3 shadowMask(vec2 uv) {
    float mask = fract(uv.x * uResolution.x * 0.333); // 3-bar mask
    if (mask < 0.333) return vec3(1.0, 0.8, 0.8);
    if (mask < 0.666) return vec3(0.8, 1.0, 0.8);
    return vec3(0.8, 0.8, 1.0);
}

// Scanline multiplier
float scanline(vec2 uv) {
    float y = uv.y * uResolution.y;
    return 1.0 - scanline_strength * sin(3.14159 * fract(y));
}

// Softening (cheap 3-tap filter)
vec3 soften(sampler2D tex, vec2 uv) {
    vec2 px = 1.0 / uResolution;
    vec3 c = texture(tex, uv).rgb * 0.60 +
             texture(tex, uv + vec2(px.x, 0)).rgb * 0.20 +
             texture(tex, uv + vec2(-px.x, 0)).rgb * 0.20;
    return c;
}

void main() {
    // Curvature
    vec2 uv = barrelDistortion(vUV);

    // Clamp to avoid sampling outside texture
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // Soften the input image
    vec3 color = mix(texture(uScene, uv).rgb, soften(uScene, uv), soften_amount);

    // Scanlines
    color *= scanline(uv);

    // Shadow mask
    color *= mix(vec3(1.0), shadowMask(uv), mask_strength);

    // Vignette
    float vign = pow(uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y), vignette_strength);
    color *= 1.0 + vign;

    FragColor = vec4(color, 1.0);
}
