#version 150
// opengl 3.2 -> glsl version1.50

in vec3 v2fColor;

uniform vec3 triangleColor;
out vec4 outColor;

void main() {
    outColor = vec4(1,1,1,1) - vec4(v2fColor, 0.1) * vec4(triangleColor, 0.1);
}
