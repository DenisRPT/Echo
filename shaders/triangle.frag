#version 330 core

in vec3 vColor;
in vec2 vPos;
out vec4 FragColor;

uniform float uTime;

void main() {
    float dist = length(vPos);
    float radius = fract(uTime * 0.4);
    float ring = smoothstep(0.06, 0.0, abs(dist - radius));

    vec3 base = vColor * 0.15;
    vec3 glow = vec3(0.3, 0.9, 1.0) * ring;
    FragColor = vec4(base + glow, 1.0);
}