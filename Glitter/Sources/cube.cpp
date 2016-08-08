#include "cube.hpp"

#include "glitter.hpp"
#include <iostream>

Cube::Cube() {
    // generate ids
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboPositions);
    glGenBuffers(1, &vboColors);
    glGenBuffers(1, &vboTextureCoords);
    glGenBuffers(1, &ebo);
    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);

    // TODO: get att locations
    GLint positionAttrLocation = 0;
    GLint colorAttrLocation = 1;
    GLint uvAttrLocation = 2;

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

    // transfer data: color attr
    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(colors),
            colors,
            GL_STATIC_DRAW);
    glVertexAttribPointer(
            colorAttrLocation,
            3,                      // nr values per vertex
            GL_FLOAT,               // data type
            GL_FALSE,               // should be normalized
            0,                      // stride (0=tight)
            0);                     // offset
    glEnableVertexAttribArray(colorAttrLocation);

    // transfer data: uv attr
    glBindBuffer(GL_ARRAY_BUFFER, vboTextureCoords);
    glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(uvs),
            uvs,
            GL_STATIC_DRAW);
    glVertexAttribPointer(
            uvAttrLocation,
            2,                      // nr values per vertex
            GL_FLOAT,               // data type
            GL_FALSE,               // should be normalized
            0,                      // stride (0=tight)
            0);                     // offset
    glEnableVertexAttribArray(uvAttrLocation);

    // transfer data: vertex indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(elements),
            elements,
            GL_STATIC_DRAW);

    // load and transfer texture1
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, texBorderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int textureWidth, textureHeight;
    int nrBytesPerPixel;
    unsigned char *image = stbi_load(
            "chaerin960x960.png",
            &textureWidth,
            &textureHeight,
            &nrBytesPerPixel,
            0);
    std::cout << textureWidth << " " << textureHeight << " " << nrBytesPerPixel << "\n";

    GLenum srcFormat = (nrBytesPerPixel == 3) ? GL_RGB : GL_RGBA;

    glTexImage2D(
            GL_TEXTURE_2D,
            0,                              // mipmap level
            GL_RGBA,                        // target format
            textureWidth, textureHeight,
            0,                              // should always be zero (legacy stuff)
            srcFormat,                      // source format
            GL_UNSIGNED_BYTE,
            image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    // load and transfer texture2
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, texBorderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    image = stbi_load(
            "sample.png",
            &textureWidth,
            &textureHeight,
            &nrBytesPerPixel,
            0);
    std::cout << textureWidth << " " << textureHeight << " " << nrBytesPerPixel << "\n";

    srcFormat = (nrBytesPerPixel == 3) ? GL_RGB : GL_RGBA;

    glTexImage2D(
            GL_TEXTURE_2D,
            0,                              // mipmap level
            GL_RGBA,                        // target format
            textureWidth, textureHeight,
            0,                              // should always be zero (legacy stuff)
            srcFormat,                        // source format
            GL_UNSIGNED_BYTE,
            image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    // end vao config
    glBindVertexArray(0);
}

Cube::~Cube() {
    glDeleteBuffers(1, &vboPositions);
    glDeleteBuffers(1, &vboColors);
    glDeleteBuffers(1, &vboTextureCoords);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    // TODO no hay delete texture?
}

void Cube::draw(Mirage::Shader *shaderPtr) {
    /*
     * TODO:
     * update transform matrices
     * bind new mat values
     */
    GLint translationMatLocation    = shaderPtr->getUniformLocation("translationMat");
    GLint rotationMatLocation       = shaderPtr->getUniformLocation("rotationMat");

    shaderPtr->bind(translationMatLocation, getTranslationMat());
    shaderPtr->bind(rotationMatLocation, getRotationMat());

    /*
     * el glActiveTexture se llama durante el draw
     * y no durante el seteo de los textureID (con el glGenTextures)
     * funciona como un glUniform, es decir, se transfiere justo antes
     * de dibujar, si se transfiere a cada slot necesario
     */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // glUniform1i(glGetUniformLocation(program, "mainTexture"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // glUniform1i(glGetUniformLocation(program, "secondTexture"), 1);
    glBindVertexArray(vao);
    glDrawElements(
            GL_TRIANGLES,
            36,                  // nr indices (and not nr triangles!)
            GL_UNSIGNED_INT,
            0);                 // offset
    glBindVertexArray(0);
}
