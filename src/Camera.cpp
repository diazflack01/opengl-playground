#include "Camera.hpp"
#include "glm/ext/matrix_transform.hpp"

template<typename T>
T clamp(const Camera::BoundedData<T>& data) {
    if (data.min.has_value() && data.val < data.min.value()) {
        return data.min.value();
    }

    if (data.max.has_value() && data.val > data.max.value()) {
        return data.max.value();
    }

    return data.val;
}

Camera::Camera(Camera::ConfigState configState) : mConfigState{configState} {
    recalculateViewMatrix();
}

void Camera::processMovement(Camera::Movement movement, float deltaTime) {
    const float cameraSpeed = mConfigState.sensitivity.movement * deltaTime;

    const auto& cameraFront = mConfigState.front;
    const auto& cameraUp = mConfigState.up;
    auto& cameraPosition = mConfigState.position;

    switch (movement) {
        case Movement::Forward:
            cameraPosition += cameraSpeed * cameraFront;
            break;
        case Movement::Backward:
            cameraPosition -= cameraSpeed * cameraFront;
            break;
        case Movement::Left:
            cameraPosition -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
            break;
        case Movement::Right:
            cameraPosition += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
            break;
    }
    recalculateViewMatrix();
}

void Camera::processMouseMovement(float xPos, float yPos) {
    glm::vec2 XYOffset = [&]{
        if (firstMouseMovement) {
            firstMouseMovement = false;
            return glm::vec2{0.0f, 0.0f};
        }
        const auto& lastXY = mConfigState.mouseXYPos;
        return glm::vec2 {xPos - lastXY.x, lastXY.y - yPos};
    }();

    // save new XY pos as last XY
    mConfigState.mouseXYPos.x = xPos;
    mConfigState.mouseXYPos.y = yPos;

    // apply sensitivity
    XYOffset *= mConfigState.sensitivity.mouse;

    mConfigState.yaw.val += XYOffset.x;
    mConfigState.pitch.val += XYOffset.y;

    mConfigState.yaw.val = clamp(mConfigState.yaw);
    mConfigState.pitch.val = clamp(mConfigState.pitch);

    glm::vec3 direction;
    direction.x = cos(glm::radians(mConfigState.yaw.val)) * cos(glm::radians(mConfigState.pitch.val));
    direction.y = sin(glm::radians(mConfigState.pitch.val));
    direction.z = sin(glm::radians(mConfigState.yaw.val)) * cos(glm::radians(mConfigState.pitch.val));
    mConfigState.front = glm::normalize(direction);

    recalculateViewMatrix();
}

void Camera::processMouseScroll(float yOffset) {
    auto& fov = mConfigState.fieldOfView;
    fov.val -= yOffset;

    fov.val = clamp(fov);
}

glm::mat4 Camera::getViewMatrix() const {
    return mViewMatrix;
}

float Camera::getFieldOfView() const {
    return mConfigState.fieldOfView.val;
}

void Camera::recalculateViewMatrix() {
    const auto& cameraPosition = mConfigState.position;
    const auto& cameraFront = mConfigState.front;
    const auto& cameraUp = mConfigState.up;
    mViewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}

glm::vec3 Camera::getPosition() const {
    return mConfigState.position;
}
