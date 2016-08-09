#ifndef CAMERA_HPP
#define CAMERA_HPP 

#include <glad/glad.h>  // it will include opengl things like GLfloat and so on
#include <glm/glm.hpp>

class Camera {
    public:
        Camera();

        // void setPosition(float x, float y, float z) { position = glm::vec3(x, y, z); }
        void setFront(float x, float y, float z);
        void setTarget(float x, float y, float z);
        void setUp(float x, float y, float z);
        void setFOV(float fov);
        void setAspectRation(float aspectRatio);
        void setNearPlane(float nearPlane);
        void setFarPlane(float farPlane);

        void rotate(glm::vec3 eulerAngles);
        /*
         * using unity order of rotation (z, x and then y)
         * TODO: ver si glm::rotateX esta usando radianes o grados
         */
        void rotate(float xAngle, float yAngle, float zAngle);

        glm::mat4 getView();
        glm::mat4 getProjection();

        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 target;   // target = position + front
        glm::vec3 up;
        float movSpeed;
        float fov;  // in angle

        // rotation euler angles
        GLfloat pitch;  // rotation x
        GLfloat yaw;    // rotation y
        GLfloat roll;   // rotation z

        bool allowMouseMov;

    private:
        // view

        // projection
        float defaultFOV;
        float aspectRatio;
        float nearPlane;
        float farPlane;
};

#endif /* CAMERA_HPP */
