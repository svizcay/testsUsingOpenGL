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
#include <camera.hpp>
#include <cube.hpp>
#include <quad.hpp>
#include <skybox.hpp>

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void cursorCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);


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
        long long nrFrames;

    private:
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point now;
        float previousTime;
        float elapsedTime;      // since the beginning

        float fps;
};

class Mouse {
    public:
        Mouse() {
            lastX = 0;
            lastY = 0;
            firstValue = true;
        }
        GLfloat lastX;
        GLfloat lastY;
        bool firstValue;
        GLfloat sensitivity = 0.05f;

    private:

};

void updateCamera(Camera & camera);

// globals
Camera camera;
Timer timer;
Mouse mouse;
bool keys[1024];

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
    glfwSetCursorPosCallback(mWindow, cursorCallback);
    glfwSetScrollCallback(mWindow, scrollCallback);
    // glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // default value -> limited to window size
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // not limited
    /*
     *  GLFW_CURSOR_DISABLED:
     *  mouse desaparece
     *  cada vez que se mueve a la izq, el valor decrementa (sin importar limites)
     *  cada vez que se mueve a la der, el valor aumenta (idem)
     *  THIS IS NEEDED FOR THE CURRENT PLAYER-CAMERA CONTROLLER
     */

    // set viewport to use full available window space
    int currentWidth, currentHeight;
    glfwGetFramebufferSize(mWindow, &currentWidth, &currentHeight);
    glViewport(0, 0, currentWidth, currentHeight);

    Mirage::Shader *shaderPtr = new Mirage::Shader();
    shaderPtr->attach("Simple/vertex.vert.glsl");
    shaderPtr->attach("Simple/fragment.frag.glsl");
    shaderPtr->link();

    Mirage::Shader *skyboxShaderPtr = new Mirage::Shader();
    skyboxShaderPtr->attach("Skybox/vertex.vert.glsl");
    skyboxShaderPtr->attach("Skybox/fragment.frag.glsl");
    skyboxShaderPtr->link();
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
    // GLint viewLocation              = shaderPtr->getUniformLocation("view");
    // GLint projectionLocation        = shaderPtr->getUniformLocation("projection");

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
    // shaderPtr->bind(viewLocation, view);
    // shaderPtr->bind(projectionLocation, projection);
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
    Cube *thirdCube = new Cube();
    thirdCube->position = glm::vec3(-5.0f, 0.0f, 5.0f);
    // Quad *quadPtr = new Quad();

    Skybox *skybox = new Skybox();

    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);   // not required. GL_LESS is the default function

    while (!glfwWindowShouldClose(mWindow)) {

        timer.update();
        glfwPollEvents();
        updateCamera(camera);

        glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT);

        // view = camera.getView();
        // projection = camera.getProjection();

        skybox->draw(skyboxShaderPtr, camera);

        // shaderPtr->bind(viewLocation, view);
        // shaderPtr->bind(projectionLocation, projection);

        firstCube->draw(shaderPtr, camera);
        secondCube->draw(shaderPtr, camera);
        thirdCube->draw(shaderPtr, camera);
        // quadPtr->draw();

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
    }

    delete firstCube;
    delete secondCube;
    delete thirdCube;
    // delete quadPtr;

    delete skybox;

    delete shaderPtr;
    delete skyboxShaderPtr;

    glfwTerminate();
    return EXIT_SUCCESS;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {

    if (action == GLFW_PRESS) {
        keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        std::cout << "esc was pressed!\n";
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        camera.allowMouseMov = !camera.allowMouseMov;
        if (camera.allowMouseMov) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouse.firstValue = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

    }

    /*
     * DONT CALL updateCamera here!
     * CALL updateCamera EVERY FRAME (using the previous stored key state).
     * when a key is pressed the input system stops
     * emitting key press for a while and then report them all (STICKY KEYS)
     */
}

void cursorCallback(GLFWwindow *window, double xpos, double ypos) {
    // (0,0) top left corner
    // std::cout << "moving cursos frame=" << timer.nrFrames << " mouse pos=(" << xpos << " " << ypos << ")\n";

    // reset mouse position
    // glfwSetCursorPos(window, mWidth / 2, mHeight / 2);

    // update camera view only is camera allow mouse movement
    if (camera.allowMouseMov) {
        if (mouse.firstValue) {
            mouse.lastX = xpos;
            mouse.lastY = ypos;
            mouse.firstValue = false;
        }

        GLfloat xOffset = xpos - mouse.lastX;
        GLfloat yOffset = mouse.lastY - ypos;

        // std::cout << "moving cursos frame=" << timer.nrFrames << " offset =(" << xOffset << " " << yOffset << ")\n";

        // update mouse object last values
        mouse.lastX = xpos;
        mouse.lastY = ypos;

        xOffset *= mouse.sensitivity;
        yOffset *= mouse.sensitivity;

        camera.yaw      += xOffset;
        camera.pitch    += yOffset;

        // vertical angle constraint
        if (camera.pitch > 89.0f) {
            camera.pitch = 89.0f;
        } else if (camera.pitch < -89.0f) {
            camera.pitch = -89.0f;
        }

        glm::vec3 front;
        front.x = cos(glm::radians(camera.pitch)) * cos(glm::radians(camera.yaw));
        front.y = sin(glm::radians(camera.pitch));
        front.z = cos(glm::radians(camera.pitch)) * sin(glm::radians(camera.yaw));
        camera.front = glm::normalize(front);
        camera.target = camera.position + camera.front;
    }

}

/*
 * cuando uno mueve la rueda hacia arriba, llega un yOffset=1
 * cuando uno mueve la rueda hacia abajo, llega un yOffset=-1
 */
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {

    // zoom in == disminuir fov (rueda hacia arriba)
    // zoom out == aumentar fov (rueda hacia abajo)

    // update fov
    camera.fov -= yOffset;

    // constraint fov (20 grades == big zoom; 80 grades == wide angle;zoom out
    if (camera.fov > 80.0f) {
        camera.fov = 80.0f;
    } else if (camera.fov < 20.0f) {
        camera.fov = 20.0f;
    }

    // std::cout << "[*] scroll (" << xOffset << ";" << yOffset << ") fov=" << camera.fov << "\n";

}

void updateCamera(Camera & camera) {
    // rotate camera along y axis
    // float targetSpeed = (M_PI / 2.0f) / 5.0f;    // (90 grados in 5 seconds)
    // camera.rotate(0.0f, targetSpeed * timer.deltaTime, 0.0f);
    // std::cout << "camera update frame=" << timer.nrFrames << "\n";

    // move camera with wasd
    if (keys[GLFW_KEY_W]) {
        camera.position += camera.front * camera.movSpeed * timer.deltaTime;
        camera.target = camera.position + camera.front;
    }
    if (keys[GLFW_KEY_S]) {
        camera.position -= camera.front * camera.movSpeed * timer.deltaTime;
        camera.target = camera.position + camera.front;
    }
    if (keys[GLFW_KEY_A]) {
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * camera.movSpeed * timer.deltaTime;
        camera.target = camera.position + camera.front;

    }
    if (keys[GLFW_KEY_D]) {
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * camera.movSpeed * timer.deltaTime;
        camera.target = camera.position + camera.front;
    }
}
