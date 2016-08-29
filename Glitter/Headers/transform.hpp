#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>

class Transform {
    public:
        Transform();
        glm::mat4 getTranslationMat();
        glm::mat4 getRotationMat();
        glm::mat4 getScaleMat();
        glm::mat4 getModelMat();
        void setParent(Transform * parent);

        glm::vec4 rotateAround(
                Transform * origin,
                glm::vec3 up,
                float amount);

        glm::vec3 position;
        glm::vec3 eulerAngles;
        glm::vec3 localScale;

        Transform *parent;

    private:
};

#endif /* TRANSFORM_HPP */
