R"(
#version 330 core

uniform float time;

out vec4 color;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800, 600);
    color = vec4(uv, 0.5+0.5*sin(time), 1.0);
}
)"
