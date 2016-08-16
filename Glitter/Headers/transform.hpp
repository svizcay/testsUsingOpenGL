#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>

class Transform {
    public:
        Transform() {}
        glm::mat4 getTranslationMat();
        glm::mat4 getRotationMat();
        glm::mat4 getScaleMat();

        glm::vec3 position;
        glm::vec3 eulerAngles;
        glm::vec3 localScale;

    private:
};

#endif /* TRANSFORM_HPP */
