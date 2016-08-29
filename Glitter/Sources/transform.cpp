#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate and glm::scale

#include <iostream>

/*
 * a partir de glm 0.9.6, funciones como glm::rotate toman siempre radianes como entrada.
 * en versiones 0.9.5 o anterior, glm::rotate tomaba eulerAngles, por lo que se debia hacer:
 *  glm::degrees(radians) o
 *  define glm_force_radians y pasar los radianes directamente a glm::rotate
 */

Transform::Transform() {
    parent = nullptr;
    localScale = glm::vec3(1, 1, 1);
}

glm::mat4 Transform::getTranslationMat() {
    return glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 Transform::getRotationMat() {
    // TODO check quaternions
    glm::mat4 mat = glm::mat4(1.0f);
    // glm::rotate toma radianes desde glm 0.9.6
    mat = glm::rotate(mat, eulerAngles.z * (float) M_PI / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    mat = glm::rotate(mat, eulerAngles.x * (float) M_PI / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    mat = glm::rotate(mat, eulerAngles.y * (float) M_PI / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    return mat;
}

glm::mat4 Transform::getScaleMat() {
    return glm::scale(glm::mat4(1.0f), localScale);
}

glm::mat4 Transform::getModelMat() {
    if (parent != nullptr) {
        return parent->getModelMat() * getTranslationMat() * getRotationMat() * getScaleMat();
    } else {
        return getTranslationMat() * getRotationMat() * getScaleMat();
    }
}

void Transform::setParent(Transform * parent) {
    this->parent = parent;
}

glm::vec4 Transform::rotateAround(
        Transform * origin,
        glm::vec3 up,
        float amount) {
    glm::mat4 rotationMat = glm::mat4(1.0f);
    if (origin != nullptr) {
        rotationMat = glm::rotate(
                origin->getModelMat(),
                amount,
                up);
    } else {
        rotationMat = glm::rotate(
                rotationMat,
                amount,
                up);
    }

    return rotationMat * glm::vec4(position, 1.0f);
}
