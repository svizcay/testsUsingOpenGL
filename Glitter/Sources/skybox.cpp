#include "skybox.hpp"

#include "glitter.hpp"
#include <iostream>
#include <vector>

Skybox::Skybox() {
    // generate ids
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboPositions);
    glGenBuffers(1, &ebo);
    glGenTextures(1, &texture);

    // TODO: get attr locations
    GLint positionAttrLocation = 0;


    // configure vao
    glBindVertexArray(vao);

    // transfer data: position attr
    glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
    glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(positions),
            positions,
            GL_STATIC_DRAW);
    glVertexAttribPointer(
            positionAttrLocation,
            3,                      // nr values per vertex
            GL_FLOAT,               // data type
            GL_FALSE,               // should be normalized
            0,                      // stride (0=tight)
            0);                     // offset
    glEnableVertexAttribArray(positionAttrLocation);

    // transfer data: vertex indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(elements),
            elements,
            GL_STATIC_DRAW);

    // load and transfer texture
    // TODO: is it glActiveTexture required?
    std::vector<const GLchar*> faces;
    faces.push_back("right.jpg");
    faces.push_back("left.jpg");
    faces.push_back("top.jpg");
    faces.push_back("bottom.jpg");
    faces.push_back("back.jpg");
    faces.push_back("front.jpg");

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int textureWidth, textureHeight;
    int nrBytesPerPixel;
    unsigned char *image;
    GLenum srcFormat;
    for (GLuint i = 0; i < faces.size(); i++) {
        image = stbi_load(
            faces[i],
            &textureWidth,
            &textureHeight,
            &nrBytesPerPixel,
            0);
        srcFormat = (nrBytesPerPixel == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, // cube slot
                0,                                  // mipmap level
                GL_RGB,                             // target format
                textureWidth, textureHeight,
                0,                                  // should always be zero (legacy stuff)
                srcFormat,                          // source format
                GL_UNSIGNED_BYTE,
                image);
        stbi_image_free(image);
    }

    // set texture parameters -> CLAMP_TO_EDGE for skyboxes
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // end vao config
    glBindVertexArray(0);
}

Skybox::~Skybox() {
    glDeleteBuffers(1, &vboPositions);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    // TODO no hay delete texture?

}

void Skybox::draw(Mirage::Shader *shaderPtr, Camera & camera) {
    /*
     * TODO:
     * update transform matrices
     * bind new mat values
     */

    /*
     * el glActiveTexture se llama durante el draw
     * y no durante el seteo de los textureID (con el glGenTextures)
     * funciona como un glUniform, es decir, se transfiere justo antes
     * de dibujar, si se transfiere a cada slot necesario
     */

    glDepthMask(GL_FALSE);

    shaderPtr->activate();

    // locations
    GLint viewLocation              = shaderPtr->getUniformLocation("view");
    GLint projectionLocation        = shaderPtr->getUniformLocation("projection");

    glm::mat4 view = glm::mat4(glm::mat3(camera.getView()));
    glm::mat4 projection = camera.getProjection();

    shaderPtr->bind(viewLocation, view);
    shaderPtr->bind(projectionLocation, projection);

    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    // glUniform1i(glGetUniformLocation(program, "mainTexture"), 0);
    glBindVertexArray(vao);
    glDrawElements(
            GL_TRIANGLES,
            36,                  // nr indices (and not nr triangles!)
            GL_UNSIGNED_INT,
            0);                 // offset
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);

}
