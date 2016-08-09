#include "camera.hpp"

#include "glitter.hpp"

Camera::Camera() {
    position    = glm::vec3(0.0f, 0.0f, 5.0f);
    front       = glm::vec3(0.0f, 0.0f, -1.0f);
    target      = position + front;
    up          = glm::vec3(0.0f, 1.0f, 0.0f);

    fov = 50.0f;    // in angles
    defaultFOV = fov;
    aspectRatio = 4.0f / 3.0f;
    nearPlane = 0.01f;
    farPlane = 100.0f;

    movSpeed = 3.0f / 1.0f; // 1 unit in 1 second

    pitch   = 0.0f;
    yaw     = 270.0f;
    roll    = 0.0f;

    allowMouseMov = true;
}

void Camera::rotate(glm::vec3 eulerAngles) {
    rotate(eulerAngles.x, eulerAngles.y, eulerAngles.z);
}

void Camera::rotate(float xAngle, float yAngle, float zAngle) {
    // update target
    glm::vec3 targetRelativeToCamera = target - position;

    // rotate z
    targetRelativeToCamera = glm::rotateZ(targetRelativeToCamera, zAngle);

    // rotate x
    targetRelativeToCamera = glm::rotateX(targetRelativeToCamera, xAngle);

    // rotate y
    targetRelativeToCamera = glm::rotateY(targetRelativeToCamera, yAngle);
    target = targetRelativeToCamera + position;
    // std::cout << "target: " << target.x << " " << target.y << " " << target.z << "\n";
}

glm::mat4 Camera::getView() {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjection() {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}
