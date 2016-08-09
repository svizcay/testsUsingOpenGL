#version 330 core
// opengl 3.2 -> glsl version1.50

in vec3 v2fUV;

uniform samplerCube mainTexture;

out vec4 outColor;

void main() {
    outColor = texture(mainTexture, v2fUV);
}
