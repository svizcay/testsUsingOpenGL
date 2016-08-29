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
#include <functional>

#include <string>
#include <sstream>

#include "shader.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "cube.hpp"
#include "quad.hpp"
#include "skybox.hpp"
#include "behaviour.hpp"

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void cursorCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

GLint textureFromFile(const char * path, std::string directory);

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

class Light : public Cube {
    public:
        Light(glm::vec3 color=glm::vec3(0.0f, 0.0f, 0.0f)) {
            this->color = color;
        }

        ~Light() {}
        glm::vec3 color;

    protected:

};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct Texture {
    GLuint id;
    std::string type;
    aiString path;
};

/*
class Mesh {
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {
            this->vertices  = vertices;
            this->indices   = indices;
            this->textures  = textures;

            // generate ids
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);

            // TODO: get attr locations
            GLint positionAttrLocation  = 0;
            GLint normalAttrLocation    = 1;
            GLint uvAttrLocation        = 2;

            // configure vao
            glBindVertexArray(vao);

            // transfer data (all at once)
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(
                    GL_ARRAY_BUFFER,
                    vertices.size() * sizeof(Vertex),
                    &vertices[0],
                    GL_STATIC_DRAW);

            // configure attr pointers

            // attr position
            glVertexAttribPointer(
                    positionAttrLocation,
                    3,                      // nr values per vertex
                    GL_FLOAT,               // data type
                    GL_FALSE,               // should be normalized
                    sizeof(Vertex),         // stride (0=tight)
                    (GLvoid*)0);            // offset
            glEnableVertexAttribArray(positionAttrLocation);

            // attr normal
            glVertexAttribPointer(
                    normalAttrLocation,
                    3,                                      // nr values per vertex
                    GL_FLOAT,                               // data type
                    GL_FALSE,                               // should be normalized
                    sizeof(Vertex),                         // stride (0=tight)
                    (GLvoid*)offsetof(Vertex, normal));     // offset
            glEnableVertexAttribArray(normalAttrLocation);

            // attr uv
            glVertexAttribPointer(
                    uvAttrLocation,
                    2,                                      // nr values per vertex
                    GL_FLOAT,                               // data type
                    GL_FALSE,                               // should be normalized
                    sizeof(Vertex),                         // stride (0=tight)
                    (GLvoid*)offsetof(Vertex, uv));         // offset
            glEnableVertexAttribArray(uvAttrLocation);

            // transfer data: vertex indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(
                    GL_ELEMENT_ARRAY_BUFFER,
                    indices.size() * sizeof(GLuint),
                    &indices[0],
                    GL_STATIC_DRAW);

            // end vao config
            glBindVertexArray(0);
        }

        ~Mesh() {
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);
        }

        void draw(Mirage::Shader *shaderPtr) {

            shaderPtr->activate();

            GLuint diffuseNr = 1;
            GLuint specularNr = 1;
            // std::cout << "mesh nr textures: " << textures.size() << "\n";
            for (GLuint i = 0; i < textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                std::stringstream ss;
                std::string number;
                std::string name = textures[i].type;
                if (name == "texture_diffuse") {
                    ss << diffuseNr++;
                } else if (name == "texture_specular") {
                    ss << specularNr++;
                }
                number = ss.str();

                // TODO: use "Shader" bind method
                // std::string uniformName = "material." + name + number;
                std::string uniformName = name + number;
                // std::cout << "uniformName: " << uniformName << "\n";
                // GLint materialLoc = shaderPtr->getUniformLocation(uniformName);
                // std::cout << "materialLoc: " << materialLoc << "\n";

                // glUniform1f(glGetUniformLocation(shaderPtr->get(), ("material." + name + number).c_str()), i);
                // shaderPtr->bind(materialLoc, i);
                // glUniform1f(glGetUniformLocation(shaderPtr->get(), (name + number).c_str()), i);
                glUniform1i(glGetUniformLocation(shaderPtr->get(), uniformName.c_str()), i);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }

            glActiveTexture(GL_TEXTURE0);

            // draw mesh
            glBindVertexArray(vao);
            glDrawElements(
                    GL_TRIANGLES,
                    indices.size(),
                    GL_UNSIGNED_INT,
                    0);
            glBindVertexArray(0);
        }

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

    private:
        GLuint vao;
        GLuint vbo;
        GLuint ebo;

};

class Model {
    public:
        Model(GLchar *path) { loadModel(path); }
        void draw(Mirage::Shader *shaderPtr) {
            for (GLuint i = 0; i < meshes.size(); i++) {
                meshes[i].draw(shaderPtr);
            }
        }

    private:
        std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(std::string path) {
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(path,
                    aiProcess_Triangulate | aiProcess_FlipUVs);
            if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
                return;
            }
            directory = path.substr(0, path.find_last_of("/"));
            std::cout << "directory: " << directory << "\n";
            processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode * node, const aiScene * scene) {
            // process node meshes
            std::cout << "nr meshes: " << node->mNumMeshes << "\n";
            for (GLuint i = 0; i < node->mNumMeshes; i++) {
                aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh, scene));
            }

            // process children
            for (GLuint i = 0; i < node->mNumChildren; i++) {
                processNode(node->mChildren[i], scene);
            }
        }

        Mesh processMesh(aiMesh * mesh, const aiScene * scene) {
            std::vector<Vertex> vertices;
            std::vector<GLuint> indices;
            std::vector<Texture> textures;

            // process vertices
            for (GLuint i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;

                glm::vec3 vector;
                // process vertex positions, normals and uvs
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.position = vector;

                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;

                if (mesh->mTextureCoords[0]) {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.uv = vec;
                } else {
                    vertex.uv = glm::vec2(0.0f, 0.0f);
                }


                vertices.push_back(vertex);
            }

            // process indices
            for (GLuint i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (GLuint j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            // process materials
            if (mesh->mMaterialIndex >= 0) {
                aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                std::vector<Texture> diffuseMaps = loadMaterialTextures(
                        material,
                        aiTextureType_DIFFUSE,
                        "texture_diffuse");
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

                std::vector<Texture> specularMaps = loadMaterialTextures(
                        material,
                        aiTextureType_SPECULAR,
                        "texture_specular");
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            }

            return Mesh(vertices, indices, textures);
        }

        std::vector<Texture> loadMaterialTextures(
                aiMaterial * material,
                aiTextureType type,
                std::string typeName)
        {
            std::cout << "getting texture of type: " << typeName << " (" << type << ")\n";
            std::vector<Texture> textures;
            for (GLuint i = 0; i < material->GetTextureCount(type); i++) {
                aiString str;
                material->GetTexture(type, i, &str);
                Texture texture;
                texture.id = textureFromFile(str.C_Str(), directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);

            }

            return textures;
        }

};
*/


void updateCamera(Camera & camera);

// globals
Camera camera;
Timer timer;
Mouse mouse;
bool keys[1024];

/*
 * create your own Behaviour that extends the base Behaviour class
 * example: a MoveCube class
 */
class MoveCube : public Behaviour {
    public:
        MoveCube(Cube & obj) : Behaviour(obj) {
            std::cout << "movecube constructor" << "\n";
            std::cout << obj.behaviours.size() << "\n";
            onStart();
        }
        void onStart() override {
            std::cout << "onStart" << "\n";
            std::cout << "object position: ";
            std::cout << object.transform.position.x << " ";
            std::cout << object.transform.position.y << " ";
            std::cout << object.transform.position.z << "\n";

            initialPosition = object.transform.position;
            targetPosition += initialPosition;
            forwardDirection = glm::normalize(targetPosition - initialPosition);
        }

        void onUpdate() override {
            // std::cout << "onUpdate" << "\n";
            if (goingForward) {
                glm::vec3 direction = targetPosition - object.transform.position;
                float distance = glm::distance(object.transform.position, targetPosition);
                bool isDistanceZero = (distance < marginError) ? true : false;
                bool isDirectionZero = (direction.x == 0 && direction.y == 0 && direction.z == 0) ? true : false;
                direction = glm::normalize(direction);

                if (isDirectionZero || isDistanceZero || (direction * -1.0f) == forwardDirection) {
                    goingForward = false;
                } else {
                    direction *= movSpeed * timer.deltaTime;
                    object.transform.position += direction;
                }
            } else {
                glm::vec3 direction = initialPosition - object.transform.position;
                float distance = glm::distance(object.transform.position, initialPosition);
                bool isDistanceZero = (distance < marginError) ? true : false;
                bool isDirectionZero = (direction.x == 0 && direction.y == 0 && direction.z == 0) ? true : false;
                direction = glm::normalize(direction);

                if (isDirectionZero || isDistanceZero || direction == forwardDirection) {
                    goingForward = true;
                } else {
                    direction *= movSpeed * timer.deltaTime;
                    object.transform.position += direction;
                }
            }
        }

        glm::vec3 targetPosition = glm::vec3(0, 0, -10);  // 10 units front
        glm::vec3 initialPosition;
        glm::vec3 forwardDirection;
        bool goingForward = true;
        /*
         * const speed
         */
        float movSpeed = 5.0f / 1.0f;   // 5 units in 1 second
        float marginError = 0.005f;
};

class RotateAround : public Behaviour {
    public:
        RotateAround(Cube & obj) : Behaviour(obj) {
            onStart();
        }

        void onStart() override {

        }

        void onUpdate() override {
            glm::vec3 angularMov = angularSpeed * timer.deltaTime * glm::vec3(0, 1, 0);
            object.transform.eulerAngles += angularMov;
            object.transform.eulerAngles.x = fmod(object.transform.eulerAngles.x, 360.f);
            object.transform.eulerAngles.y = fmod(object.transform.eulerAngles.y, 360.f);
            object.transform.eulerAngles.z = fmod(object.transform.eulerAngles.z, 360.f);

            // std::cout << "eulerAngles: ";
            // std::cout << object.transform.eulerAngles.x << " ";
            // std::cout << object.transform.eulerAngles.y << " ";
            // std::cout << object.transform.eulerAngles.z << "\n";
        }

        // Cube & targetGameObject;
        float angularSpeed = 360.0f / 1.0f;   // 360 degree in 1 second
};


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

    Mirage::Shader *meshShaderPtr = new Mirage::Shader();
    meshShaderPtr->attach("Mesh/vertex.vert.glsl");
    meshShaderPtr->attach("Mesh/fragment.frag.glsl");
    meshShaderPtr->link();

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
    secondCube->transform.position = glm::vec3(5.0f, 0.0f, 5.0f);
    Cube *thirdCube = new Cube();
    thirdCube->transform.position = glm::vec3(-5.0f, 0.0f, 5.0f);
    // Quad *quadPtr = new Quad();

    MoveCube moveCubeBehaviour (*thirdCube);
    RotateAround rotateAroundBehaviour (*secondCube);

    // std::cout << "size of cube in bytes: " << sizeof(Cube) << "\n";
    // std::cout << "memory address using printf and %p" << "\n";
    // printf("%p firstCube\n", (void *)firstCube);
    // printf("%p secondCube\n", (void *)secondCube);
    // std::cout << "memory address using iostream" << "\n";
    // std::cout << firstCube << " firstCube" << "\n";
    // std::cout << secondCube << " secondCube" << "\n";


    Skybox *skybox = new Skybox();

    // Model model1 ("nanosuit/nanosuit.obj");
    Mirage::Mesh *mesh1 = new Mirage::Mesh("nanosuit2/nanosuit2.obj");

    Light *light = new Light();
    light->transform.position = glm::vec3(5.0f, 0.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);   // not required. GL_LESS is the default function

    while (!glfwWindowShouldClose(mWindow)) {

        timer.update();
        glfwPollEvents();
        updateCamera(camera);

        glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT);

        view = camera.getView();
        projection = camera.getProjection();

        skybox->draw(skyboxShaderPtr, camera);

        // shaderPtr->bind(viewLocation, view);
        // shaderPtr->bind(projectionLocation, projection);

        // firstCube->draw(shaderPtr, camera);
        secondCube->update();
        secondCube->draw(shaderPtr, camera);
        thirdCube->update();
        thirdCube->draw(shaderPtr, camera);
        // quadPtr->draw();

        light->draw(shaderPtr, camera);


        meshShaderPtr->activate();
        GLint modelLocation         = meshShaderPtr->getUniformLocation("model");
        GLint viewLocation          = meshShaderPtr->getUniformLocation("view");
        GLint projectionLocation    = meshShaderPtr->getUniformLocation("projection");

        // std::cout << "locations: " << modelLocation << " " << viewLocation << " " << projectionLocation << "\n";

        meshShaderPtr->bind(modelLocation, glm::mat4());
        meshShaderPtr->bind(viewLocation, view);
        meshShaderPtr->bind(projectionLocation, projection);

        // model1.draw(meshShaderPtr);
        mesh1->draw(meshShaderPtr->get());

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
    }

    delete firstCube;
    delete secondCube;
    delete thirdCube;
    // delete quadPtr;

    delete light;

    delete skybox;

    delete mesh1;

    delete shaderPtr;
    delete skyboxShaderPtr;
    delete meshShaderPtr;

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

GLint textureFromFile(const char * path, std::string directory) {
    std::string filename = std::string(path);
    filename = directory + "/" + filename;
    std::cout << "texture filename: " << filename << "\n";

    GLuint textureID;
    glGenTextures(1, &textureID);
    int textureWidth, textureHeight;
    int nrBytesPerPixel;
    // unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    unsigned char *image = stbi_load(
            filename.c_str(),
            &textureWidth,
            &textureHeight,
            &nrBytesPerPixel,
            0);
    GLenum srcFormat = (nrBytesPerPixel == 3) ? GL_RGB : GL_RGBA;
    // std::cout << textureWidth << "x" << textureHeight << " with " << nrBytesPerPixel << " channels\n";
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            textureWidth, textureHeight,
            0,
            srcFormat,
            GL_UNSIGNED_BYTE,
            image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // SOIL_free_image_data(image);
    stbi_image_free(image);
    return textureID;
}
