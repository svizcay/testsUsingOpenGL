#ifndef CUBE_HPP
#define CUBE_HPP

#include <glad/glad.h>  // it will include opengl things like GLfloat and so on

#include <vector>

#include <transform.hpp>
#include <shader.hpp>
#include <camera.hpp>

// class Vertex {
//     glm::vec3 position;
//     glm::vec4 color;
//     glm::vec2 uv;
// };

class Behaviour;

class Cube {
    public:
        Cube();
        ~Cube();
        void draw(Mirage::Shader *shaderPtr, Camera & camera);
        void update();

        Transform transform;
        std::vector<Behaviour*> behaviours;
    protected:

        // GLfloat positions[24] = {
        //     // front face
        //     -0.5f,  -0.5f,  0.5f,
        //     0.5f,   -0.5f,  0.5f,
        //     0.5f,   0.5f,   0.5f,
        //     -0.5f,  0.5f,   0.5f,
        //     // back face
        //     -0.5f,  -0.5f,  -0.5f,
        //     0.5f,   -0.5f,  -0.5f,
        //     0.5f,   0.5f,   -0.5f,
        //     -0.5f,  0.5f,   -0.5f,
        // };

        GLfloat positions[144] = {
            // front face
            -0.5f,  -0.5f,  0.5f,   // A
            0.5f,   -0.5f,  0.5f,   // B
            0.5f,   0.5f,   0.5f,   // C
            -0.5f,  -0.5f,  0.5f,   // A
            0.5f,   0.5f,   0.5f,   // C
            -0.5f,  0.5f,   0.5f,   // D
            // left face
            -0.5f,  -0.5f,  -0.5f,  // E
            -0.5f,  -0.5f,  0.5f,   // A
            -0.5f,  0.5f,   0.5f,   // D
            -0.5f,  -0.5f,  -0.5f,  // E
            -0.5f,  0.5f,   0.5f,   // D
            -0.5f,  0.5f,   -0.5f,  // H
            // right face
            0.5f,   -0.5f,  0.5f,   // B
            0.5f,   -0.5f,  -0.5f,  // F
            0.5f,   0.5f,   -0.5f,  // G
            0.5f,   -0.5f,  0.5f,   // B
            0.5f,   0.5f,   -0.5f,  // G
            0.5f,   0.5f,   0.5f,   // C
            // back face
            0.5f,   -0.5f,  -0.5f,  // F
            -0.5f,  -0.5f,  -0.5f,  // E
            -0.5f,  0.5f,   -0.5f,  // H
            0.5f,   -0.5f,  -0.5f,  // F
            -0.5f,  0.5f,   -0.5f,  // H
            0.5f,   0.5f,   -0.5f,  // G
            // top face
            -0.5f,  0.5f,   0.5f,   // D
            0.5f,   0.5f,   0.5f,   // C
            0.5f,   0.5f,   -0.5f,  // G
            -0.5f,  0.5f,   0.5f,   // D
            0.5f,   0.5f,   -0.5f,  // G
            -0.5f,  0.5f,   -0.5f,  // H
            // bottom face
            -0.5f,  -0.5f,  -0.5f,  // E
            0.5f,   -0.5f,  -0.5f,  // F
            0.5f,   -0.5f,  0.5f,   // B
            -0.5f,  -0.5f,  -0.5f,  // E
            0.5f,   -0.5f,  0.5f,   // B
            -0.5f,  -0.5f,  0.5f,   // A
        };

        // GLfloat colors[24] = {
        //     // front face
        //     1.0f, 0.0f, 0.0f,
        //     1.0f, 0.0f, 0.0f,
        //     1.0f, 0.0f, 0.0f,
        //     1.0f, 0.0f, 0.0f,
        //     // back face
        //     0.0f, 1.0f, 0.0f,
        //     0.0f, 1.0f, 0.0f,
        //     0.0f, 1.0f, 0.0f,
        //     0.0f, 1.0f, 0.0f,
        // };

        GLfloat colors[144] = {
            // front face (red)
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            // left face (green)
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            // right face (blue)
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            // back face (yellow)
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            // top face (cyan = sky blue)
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            // bottom face (magenta = pink)
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
        };

        // GLfloat uvs[16] = {
        //     // front face
        //     0.0f, 0.0f,
        //     1.0f, 0.0f,
        //     1.0f, 1.0f,
        //     0.0f, 1.0f,
        //     // back face
        //     0.0f, 0.0f,
        //     1.0f, 0.0f,
        //     1.0f, 1.0f,
        //     0.0f, 1.0f,
        // };

        GLfloat uvs[72] = {
            // front face
            0.0f, 0.0f, // A
            1.0f, 0.0f, // B
            1.0f, 1.0f, // C
            0.0f, 0.0f, // A
            1.0f, 1.0f, // C
            0.0f, 1.0f, // D
            // left face
            0.0f, 0.0f, // A
            1.0f, 0.0f, // B
            1.0f, 1.0f, // C
            0.0f, 0.0f, // A
            1.0f, 1.0f, // C
            0.0f, 1.0f, // D
            // right face
            0.0f, 0.0f, // A
            1.0f, 0.0f, // B
            1.0f, 1.0f, // C
            0.0f, 0.0f, // A
            1.0f, 1.0f, // C
            0.0f, 1.0f, // D
            // back face
            0.0f, 0.0f, // A
            1.0f, 0.0f, // B
            1.0f, 1.0f, // C
            0.0f, 0.0f, // A
            1.0f, 1.0f, // C
            0.0f, 1.0f, // D
            // top face
            0.0f, 0.0f, // A
            1.0f, 0.0f, // B
            1.0f, 1.0f, // C
            0.0f, 0.0f, // A
            1.0f, 1.0f, // C
            0.0f, 1.0f, // D
            // bottom face
            0.0f, 0.0f, // A
            1.0f, 0.0f, // B
            1.0f, 1.0f, // C
            0.0f, 0.0f, // A
            1.0f, 1.0f, // C
            0.0f, 1.0f, // D
        };

        // GLuint elements[36] = {
        //     0,1,2,0,2,3,    // 1st face
        //     4,0,3,4,3,7,    // left face
        //     5,1,2,5,2,6,    // right face
        //     4,5,6,4,6,7,    // back face
        //     3,2,6,3,6,7,    // top face
        //     0,1,5,0,5,4,    // bottom face
        // };

        GLuint elements[36] = {
            0,1,2,3,4,5,        // 1st face
            6,7,8,9,10,11,      // left face
            12,13,14,15,16,17,  // right face
            18,19,20,21,22,23,  // back face
            24,25,26,27,28,29,  // top face
            30,31,32,33,34,35,  // bottom face
        };

        GLfloat texBorderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

        GLuint vao;
        GLuint vboPositions;
        GLuint vboNormals;
        GLuint vboColors;
        GLuint vboTextureCoords;
        GLuint ebo;

        GLuint texture1;
        GLuint texture2;
};

#endif /* CUBE_HPP */
