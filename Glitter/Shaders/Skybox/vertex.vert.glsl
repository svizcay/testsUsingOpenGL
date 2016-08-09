#version 330 core
// opengl 3.2 -> glsl version1.50

in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 v2fUV;

void main() {

    gl_Position = projection * view * vec4(position, 1);
    // gl_Position = vec4(position, 1);
    v2fUV = position;
}
