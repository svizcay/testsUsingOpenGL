// Local Headers
#include "glitter.hpp"

// System Headers
/*
 * glad is an opengl function loader (better than glew)
 * it must be included before glfw
*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <chrono>

#include <shader.hpp>

int main(int argc, char * argv[]) {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);   // windowed
    // auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", glfwGetPrimaryMonitor(), nullptr);   // fullscreen

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    GLfloat vertices[12] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
    };

    GLfloat colors[12] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
    };

    GLuint elements[6] = {
        0, 1, 2, 0, 2, 3,
    };

    Mirage::Shader *shaderPtr = new Mirage::Shader();
    shaderPtr->attach("Simple/vertex.vert.glsl");
    shaderPtr->attach("Simple/fragment.frag.glsl");
    shaderPtr->link();
    shaderPtr->activate();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint verticesVBO;
    GLuint colorsVBO;
    GLuint elementsVBO;
    glGenBuffers(1, &verticesVBO);
    glGenBuffers(1, &colorsVBO);
    glGenBuffers(1, &elementsVBO);

    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    // std::cout << "sizeof(vertices): " << sizeof(vertices) << "\n";
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    GLint positionAttrLocation = shaderPtr->getAttrLocation("position");
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);  // this info is stored when a vao is bound
    glEnableVertexAttribArray(positionAttrLocation);

    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    GLint colorAttrLocation = shaderPtr->getAttrLocation("color");
    glVertexAttribPointer(
            colorAttrLocation,  // attr index
            3,                  // nr of values
            GL_FLOAT,           // data type
            GL_FALSE,           // should those values be normalized to [-1:1] if they aren't floats
            0,                  // stride: distance between "vertices data" eg: (x,y,r,g,b) -> stride=5*sizeof(float)
            0);                 // offset eg: if color in (x,y,r,g,b) -> offset=2*sizeof(float)
    glEnableVertexAttribArray(colorAttrLocation);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


    // set background Fill Color
    glm::vec3 feldrgau = glm::vec3(0.302, 0.365, 0.325);
    glClearColor(feldrgau.r, feldrgau.g, feldrgau.b, 1.0f);

    GLint triangleColorLocation = shaderPtr->getUniformLocation("triangleColor");
    GLint thetaLocation = shaderPtr->getUniformLocation("theta");
    GLint xOffsetLocation = shaderPtr->getUniformLocation("xOffset");
    GLint translationMatLocation = shaderPtr->getUniformLocation("translationMat");

    /*
     * glm::mat 4 each item is a glm::vec4 col
     */
    glm::mat4 translationMat = glm::mat4(
            glm::vec4(1,0,0,0),
            glm::vec4(0,1,0,0),
            glm::vec4(0,0,1,0),
            glm::vec4(1,0,0,1)
            );
    shaderPtr->bind(translationMatLocation, translationMat);

    shaderPtr->bind(triangleColorLocation, 1.0f, 0.0f, 0.0f);

    auto timeStart = std::chrono::high_resolution_clock::now();
    float previousTime = 0.0f;
    long long nrFrames = 1;

    float colorTransitionTime = 0.0f;
    float targetTransitionTime = 5.0f;  // in 5 seconds it will go from 0 to 1;

    float theta = 0.0f;
    float rotationTargetTime = 5.0f;    // turn 360 in 2 seconds

    // Rendering Loop
    while (!glfwWindowShouldClose(mWindow)) {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(mWindow, true);
        }
        auto timeNow = std::chrono::high_resolution_clock::now();
        float elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(timeNow-timeStart).count();
        float deltaTime = elapsedTime - previousTime;
        previousTime = elapsedTime;
        float fps = nrFrames / elapsedTime;
        nrFrames++;

        colorTransitionTime += M_PI * deltaTime / targetTransitionTime;
        float channelValue = cos(colorTransitionTime) * 0.5f + 0.5f;

        theta += 2 * M_PI * deltaTime / rotationTargetTime;

        shaderPtr->bind(triangleColorLocation, channelValue, channelValue, channelValue);
        shaderPtr->bind(thetaLocation, theta);
        shaderPtr->bind(xOffsetLocation, sin(elapsedTime));
        // std::cout << "elapsedTime: " << elapsedTime << "\n";

        glClear(GL_COLOR_BUFFER_BIT);

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(
                GL_TRIANGLES,
                6,                  // nr indices (and not nr triangles!)
                GL_UNSIGNED_INT,
                0);                 // offset

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &colorsVBO);
    glDeleteBuffers(1, &elementsVBO);
    glDeleteVertexArrays(1, &vao);
    delete shaderPtr;

    glfwTerminate();
    return EXIT_SUCCESS;
}
