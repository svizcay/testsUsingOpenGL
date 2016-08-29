#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <glad/glad.h>  // it will include opengl things like GLfloat and so on

class Mouse {
    public:
        Mouse();
        GLfloat lastX;
        GLfloat lastY;
        bool firstValue;
        GLfloat sensitivity = 0.05f;

    private:

};

#endif /* MOUSE_HPP */
