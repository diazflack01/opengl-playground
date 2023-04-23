#include "Bone.hpp"

#include <assimp/anim.h>
#include <glm/gtx/quaternion.hpp>

constexpr glm::mat4 IDENTITY_MATRIX{1.0f};

Bone::Bone(std::string name, int id, const aiNodeAnim *channel)
 : m_name{std::move(name)}
 , m_Id{id}
{
    // positions
    for (int idx = 0; idx < channel->mNumPositionKeys; ++idx) {
        const auto& positionKey = channel->mPositionKeys[idx];
        KeyPosition data{};
        data.position = glm::vec3{positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z};
        data.timeStamp = positionKey.mTime;
        m_positions.push_back(data);
    }

    // rotations
    for (int idx = 0; idx < channel->mNumRotationKeys; ++idx) {
        const auto& rotationKey = channel->mRotationKeys[idx];
        KeyRotation data{};
        data.orientation = glm::quat{rotationKey.mValue.w, rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z};
        data.timeStamp = rotationKey.mTime;
        m_rotations.push_back(data);
    }

    // scales
    for (int idx = 0; idx < channel->mNumScalingKeys; ++idx) {
        const auto& scalingKey = channel->mScalingKeys[idx];
        KeyScale data{};
        data.scale = glm::vec3{scalingKey.mValue.x, scalingKey.mValue.y, scalingKey.mValue.z};
        data.timeStamp = scalingKey.mTime;
        m_scales.push_back(data);
    }
}

void Bone::update(float animationTime) {
    const auto translation = interpolatePosition(animationTime);
    const auto rotation = interpolateRotation(animationTime);
    const auto scale = interpolateScale(animationTime);
    m_localTransform = translation * rotation * scale;
}

glm::mat4 Bone::getLocalTransform() {
    return m_localTransform;
}

std::string Bone::getBoneName() const {
    return m_name;
}

int Bone::getBoneId() {
    return m_Id;
}

template<typename T>
int getImpl(float animationTime, const std::vector<T>& container) {
    for (auto idx = 0; idx < container.size() - 1; ++idx) {
        if (animationTime < container[idx + 1].timeStamp) {
            return idx;
        }
    }
    assert(false);
}

int Bone::getPositionIdx(float animationTime) {
    return getImpl(animationTime, m_positions);
}

int Bone::getRotationIdx(float animationTime) {
    return getImpl(animationTime, m_rotations);
}

int Bone::getScaleIdx(float animationTime) {
    return getImpl(animationTime, m_scales);
}

float Bone::getScaleFactor(float lastTimestamp, float nextTimestamp, float animationTime) {
    const auto midwayLength = animationTime - lastTimestamp;
    const auto framesDiff = nextTimestamp - lastTimestamp;
    const float scaleFactor = midwayLength / framesDiff;
    return scaleFactor;
}

glm::mat4 Bone::interpolatePosition(float animationTime) {
    if (m_positions.size() == 1) {
        return glm::translate(IDENTITY_MATRIX, m_positions[0].position);
    }

    const auto p0Idx = getPositionIdx(animationTime);
    const auto p1Idx = p0Idx + 1;
    const auto scaleFactor = getScaleFactor(m_positions[p0Idx].timeStamp, m_positions[p1Idx].timeStamp, animationTime);
    const auto finalPosition = glm::mix(m_positions[p0Idx].position, m_positions[p1Idx].position, scaleFactor);
    return glm::translate(IDENTITY_MATRIX, finalPosition);
}

glm::mat4 Bone::interpolateRotation(float animationTime) {
    if (m_rotations.size() == 1) {
        const auto rotation = glm::normalize(m_rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    const auto p0Idx = getRotationIdx(animationTime);
    const auto p1Idx = p0Idx + 1;
    const auto scaleFactor = getScaleFactor(m_rotations[p0Idx].timeStamp, m_rotations[p1Idx].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(m_rotations[p0Idx].orientation, m_rotations[p1Idx].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::interpolateScale(float animationTime) {
    if (m_scales.size() == 1) {
        return glm::scale(IDENTITY_MATRIX, m_scales[0].scale);
    }

    const auto p0Idx = getScaleIdx(animationTime);
    const auto p1Idx = p0Idx + 1;
    const auto scaleFactor = getScaleFactor(m_scales[p0Idx].timeStamp, m_scales[p1Idx].timeStamp, animationTime);
    const auto finalScale = glm::mix(m_scales[p0Idx].scale, m_scales[p1Idx].scale, scaleFactor);
    return glm::scale(IDENTITY_MATRIX, finalScale);
}
