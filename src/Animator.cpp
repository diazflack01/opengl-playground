#include "Animator.hpp"

#include "Animation.hpp"
#include "Bone.hpp"

Animator::Animator(Animation *animation) {
    m_currentAnimation = animation;
    m_finalBoneMatrices.reserve(100);

    for (auto i = 0; i < 100; i++) {
        m_finalBoneMatrices.emplace_back(glm::mat4{1.0f});
    }
}

void Animator::updateAnimation(float dt) {
    m_deltaTime = dt;
    if (m_currentAnimation != nullptr) {
        m_currentTime += m_currentAnimation->getTicksPerSecond() * dt;
        m_currentTime = fmod(m_currentTime, m_currentAnimation->getDuration());
        calculateBoneTransform(&m_currentAnimation->getRootNode(), glm::mat4{1.0f});
    }
}

void Animator::playAnimation(Animation *animation) {
    m_currentAnimation = animation;
    m_currentTime = 0.0f;
}

void Animator::calculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform) {
    const auto nodeName = node->name;
    glm::mat4 nodeTransform = node->transform;

    auto bone = m_currentAnimation->findBone(nodeName);
    if (bone.has_value()) {
        bone->update(m_currentTime);
        nodeTransform = bone->getLocalTransform();
    }

    const glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_currentAnimation->getBoneIdMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        const int index = boneInfoMap[nodeName].id;
        const glm::mat4 offset = boneInfoMap[nodeName].offsetMatrix;
        m_finalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++) {
        calculateBoneTransform(&node->children[i], globalTransformation);
    }
}

std::vector<glm::mat4> Animator::getFinalBoneMatrices() {
    return m_finalBoneMatrices;
}
