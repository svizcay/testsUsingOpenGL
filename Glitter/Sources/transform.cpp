#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate and glm::scale

glm::mat4 Transform::getTranslationMat() {
    return glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 Transform::getRotationMat() {
    // TODO check quaternions
    glm::mat4 mat = glm::mat4(1.0f);
    mat = glm::rotate(mat, eulerAngles.z * 180.0f / (float) M_PI, glm::vec3(0.0f, 0.0f, 1.0f));
    mat = glm::rotate(mat, eulerAngles.x * 180.0f / (float) M_PI, glm::vec3(1.0f, 0.0f, 0.0f));
    mat = glm::rotate(mat, eulerAngles.y * 180.0f / (float) M_PI, glm::vec3(0.0f, 1.0f, 0.0f));
    return mat;
}

glm::mat4 Transform::getScaleMat() {
    return glm::scale(glm::mat4(1.0f), localScale);
}
