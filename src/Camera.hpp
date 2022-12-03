#pragma once

#include <optional>
#include <glm/glm.hpp>

class Camera {
public:
    enum class Movement {
        Forward,
        Backward,
        Left,
        Right
    };

    template<typename T>
    struct BoundedData {
        T val;
        std::optional<T> min;
        std::optional<T> max;
    };

    struct Sensitivity {
        float movement;
        float mouse;
    };

    struct ConfigState {
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        BoundedData<float> fieldOfView;
        BoundedData<float> pitch;
        BoundedData<float> yaw;
        Sensitivity sensitivity;
        glm::vec2 mouseXYPos;
    };

    explicit Camera(ConfigState configState);

    void processMovement(Movement movement, float deltaTime);

    void processMouseMovement(float xPos, float yPos);

    void processMouseScroll(float yOffset);

    glm::mat4 getViewMatrix() const;

    float getFieldOfView() const;

private:
    void recalculateViewMatrix();

    ConfigState mConfigState;
    bool firstMouseMovement{true};
    glm::mat4 mViewMatrix{1.0};
};
