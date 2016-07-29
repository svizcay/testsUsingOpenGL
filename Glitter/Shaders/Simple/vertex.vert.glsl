#version 150
// opengl 3.2 -> glsl version1.50

in vec3 position;
in vec3 color;

out vec3 v2fColor;

uniform float theta;
uniform float xOffset;
uniform mat4 translationMat;

void main() {
    mat4 rotationMat = mat4(
            vec4(cos(theta),sin(theta),0,0),                 // 1st column
            vec4(-sin(theta),cos(theta),0,0),                 // 2nd column
            vec4(0,0,1,0),                 // 3rd column
            vec4(0,0,0,1)                 // 4th column
            );
    /*
     * mat4 each item is a vec4 col
     */
    // mat4 translationMat = mat4(
    //         vec4(1, 0, 0, 0),
    //         vec4(0, 1, 0, 0),
    //         vec4(0, 0, 1, 0),
    //         vec4(xOffset, 0, 0, 1)
    //         );
    // float a = cos(theta);
    gl_Position = translationMat * rotationMat * vec4(position, 1);
    v2fColor = color;
}
