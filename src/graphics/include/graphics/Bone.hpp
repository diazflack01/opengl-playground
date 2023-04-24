#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

struct aiNodeAnim;

struct KeyPosition {
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    float timeStamp;
};

class Bone {
public:
    Bone(std::string name, int id, const aiNodeAnim* channel);

    void update(float animationTime);

    glm::mat4 getLocalTransform();
    [[nodiscard]] std::string getBoneName() const;
    int getBoneId();
    int getPositionIdx(float animationTime);
    int getRotationIdx(float animationTime);
    int getScaleIdx(float animationTime);

private:
    float getScaleFactor(float lastTimestamp, float nextTimestamp, float animationTime);
    glm::mat4 interpolatePosition(float animationTime);
    glm::mat4 interpolateRotation(float animationTime);
    glm::mat4 interpolateScale(float animationTime);

    std::vector<KeyPosition> m_positions;
    std::vector<KeyRotation> m_rotations;
    std::vector<KeyScale> m_scales;

    std::string m_name;
    int m_Id;

    glm::mat4 m_localTransform{1.0f};
};