#version 330 core
// opengl 3.2 -> glsl version1.50

in vec3 position;
in vec3 color;
in vec2 uv;

uniform mat4 translationMat;
uniform mat4 rotationMat;

uniform mat4 view;
uniform mat4 projection;

out vec3 v2fColor;
out vec2 v2fUV;

void main() {

    mat4 modelMat = translationMat * rotationMat;
    gl_Position = projection * view * modelMat * vec4(position, 1);
    v2fColor = color;
    v2fUV = uv;
}
