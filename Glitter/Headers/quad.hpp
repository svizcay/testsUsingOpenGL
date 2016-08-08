#ifndef QUAD_HPP
#define QUAD_HPP

#include <glad/glad.h>  // it will include opengl things like GLfloat and so on

class Quad {
    public:
        Quad();
        ~Quad();
        void draw();

    private:
        GLfloat positions[12] = {
            // front face
            -0.5f,  -0.5f,  0.0f,
            0.5f,   -0.5f,  0.0f,
            0.5f,   0.5f,   0.0f,
            -0.5f,  0.5f,   0.0f,
        };

        GLfloat colors[12] = {
            // front face
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
        };

        GLfloat uvs[16] = {
            // front face
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
        };

        GLuint elements[6] = {
            0,1,2,0,2,3,    // 1st face
        };

        GLfloat texBorderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};

        GLuint vao;
        GLuint vboPositions;
        GLuint vboColors;
        GLuint vboTextureCoords;
        GLuint ebo;

        GLuint texture1;
        GLuint texture2;
};

#endif /* QUAD_HPP */
