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
#include <cube.hpp>
#include <quad.hpp>

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);


class Camera {
    public:
        Camera() {
            position    = glm::vec3(0.0f, 5.0f, 20.0f);
            target      = glm::vec3(0.0f, 0.0f, 0.0f);
            up          = glm::vec3(0.0f, 1.0f, 0.0f);

            fov = 45.0f;
            aspectRatio = 4.0f / 3.0f;
            nearPlane = 0.01f;
            farPlane = 100.0f;
        }

        void setPosition(float x, float y, float z);
        void setTarget(float x, float y, float z);
        void setUp(float x, float y, float z);
        void setFOV(float fov);
        void setAspectRation(float aspectRatio);
        void setNearPlane(float nearPlane);
        void setFarPlane(float farPlane);

        void rotate(glm::vec3 eulerAngles) {
            rotate(eulerAngles.x, eulerAngles.y, eulerAngles.z);
        }
        /*
         * using unity order of rotation (z, x and then y)
         * TODO: ver si glm::rotateX esta usando radianes o grados
         */
        void rotate(float xAngle, float yAngle, float zAngle) {
            // update target
            glm::vec3 targetRelativeToCamera = target - position;

            // rotate z
            targetRelativeToCamera = glm::rotateZ(targetRelativeToCamera, zAngle);

            // rotate x
            targetRelativeToCamera = glm::rotateX(targetRelativeToCamera, xAngle);

            // rotate y
            targetRelativeToCamera = glm::rotateY(targetRelativeToCamera, yAngle);
            target = targetRelativeToCamera;
            std::cout << "target: " << target.x << " " << target.y << " " << target.z << "\n";
        }

        glm::mat4 getView() {
            return glm::lookAt(position, target, up);
        }

        glm::mat4 getProjection() {
            return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
        }

    private:
        // view
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;

        // projection
        float fov;  // in angle
        float aspectRatio;
        float nearPlane;
        float farPlane;
};

class Timer {
    public:
        Timer() {
            start = std::chrono::high_resolution_clock::now();
            previousTime = 0.0f;
            nrFrames = 0;
        }

        void update() {
            now = std::chrono::high_resolution_clock::now();
            elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(now-start).count();
            deltaTime = elapsedTime - previousTime;
            previousTime = elapsedTime;
            nrFrames++;
            fps = nrFrames / elapsedTime;
        }

        float deltaTime;        // since last frame

    private:
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point now;
        float previousTime;
        float elapsedTime;      // since the beginning

        long long nrFrames;
        float fps;
};

void updateCamera(Camera & camera);

// globals
Camera camera;
Timer timer;

int main(int argc, char * argv[]) {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL with offset viewport", nullptr, nullptr);   // windowed
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

    glfwSetKeyCallback(mWindow, keyCallback);

    // set viewport to use full available window space
    int currentWidth, currentHeight;
    glfwGetFramebufferSize(mWindow, &currentWidth, &currentHeight);
    glViewport(0, 0, currentWidth, currentHeight);

    Mirage::Shader *shaderPtr = new Mirage::Shader();
    shaderPtr->attach("Simple/vertex.vert.glsl");
    shaderPtr->attach("Simple/fragment.frag.glsl");
    shaderPtr->link();
    /*
     * finding a uniform doesn't require a program to be active
     * but setting its value does it
     */
    shaderPtr->activate();

    // set background Fill Color
    glm::vec3 feldrgau = glm::vec3(0.302, 0.365, 0.325);
    glClearColor(feldrgau.r, feldrgau.g, feldrgau.b, 1.0f);

    // uniform location
    GLint baseColorLocation         = shaderPtr->getUniformLocation("baseColor");
    GLint translationMatLocation    = shaderPtr->getUniformLocation("translationMat");
    GLint rotationMatLocation       = shaderPtr->getUniformLocation("rotationMat");
    GLint viewLocation              = shaderPtr->getUniformLocation("view");
    GLint projectionLocation        = shaderPtr->getUniformLocation("projection");

    /*
     * glm::mat 4 each item is a glm::vec4 col
     */
    glm::mat4 translationMat = glm::mat4(
            glm::vec4(1,0,0,0),
            glm::vec4(0,1,0,0),
            glm::vec4(0,0,1,0),
            glm::vec4(0,0,0,1)
            );

    glm::mat4 rotationMat;

    glm::mat4 view = camera.getView();
    glm::mat4 projection = camera.getProjection();

    shaderPtr->bind(translationMatLocation, translationMat);
    shaderPtr->bind(rotationMatLocation, rotationMat);
    shaderPtr->bind(viewLocation, view);
    shaderPtr->bind(projectionLocation, projection);
    shaderPtr->bind(baseColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

    float colorTransitionTime = 0.0f;
    float targetTransitionTime = 5.0f;  // in 5 seconds it will go from 0 to 1;

    float rotationTargetTime = 5.0f;    // turn 360 in 2 seconds

    // set drawing mode: GL_LINE (wireframe) or GL_FILL (default)
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // default value

    // query hardware max nrVertexAttributes
    // GLint nrAttributes;
    // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    // std::cout << "max nr (4-component) attributes: " << nrAttributes << "\n";

    // Rendering Loop

    Cube *firstCube = new Cube();
    Cube *secondCube = new Cube();
    secondCube->position = glm::vec3(5.0f, 0.0f, 5.0f);
    // Quad *quadPtr = new Quad();

    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);   // not required. GL_LESS is the default function

    while (!glfwWindowShouldClose(mWindow)) {

        glfwPollEvents();
        timer.update();
        updateCamera(camera);

        // rotationMat = glm::rotate(
        //         rotationMat,
        //         deltaTime,
        //         glm::vec3(0.0f, 0.0f, 1.0f)
        // );

        // std::cout << "elapsedTime: " << elapsedTime << "\n";

        glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT);


        // update Camera
        // glm::vec3 newCameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        // glm::mat4 cameraRotationMat = glm::rotate(glm::mat4(), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // for (int col = 0; col < 4; col++) {
        //     for (int row = 0; row < 4; row++) {
        //         std::cout << cameraRotationMat[col][row] << " ";
        //     }
        //     std::cout << "\n";
        // }
        // glm::vec4 newCameraTargetWithW = cameraRotationMat * glm::vec4(newCameraTarget, 1.0f);
        // std::cout << newCameraTargetWithW[0] << " ";
        // std::cout << newCameraTargetWithW[1] << " ";
        // std::cout << newCameraTargetWithW[2] << " ";
        // std::cout << newCameraTargetWithW[3] << "\n";

        view = camera.getView();
        shaderPtr->bind(viewLocation, view);

        firstCube->draw(shaderPtr);
        secondCube->draw(shaderPtr);
        // quadPtr->draw();

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
    }

    delete firstCube;
    delete secondCube;
    // delete quadPtr;

    delete shaderPtr;

    glfwTerminate();
    return EXIT_SUCCESS;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        std::cout << "esc was pressed!\n";
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void updateCamera(Camera & camera) {
    // rotate camera along y axis
    float targetSpeed = (M_PI / 2.0f) / 1.0f;    // (90 grados in 1 seconds)
    camera.rotate(0.0f, targetSpeed * timer.deltaTime, 0.0f);
}
