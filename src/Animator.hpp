#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

class Animation;
struct AssimpNodeData;

class Animator {
public:
    Animator(Animation *animation);

    void updateAnimation(float dt);

    void playAnimation(Animation *animation);

    void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    std::vector<glm::mat4> getFinalBoneMatrices();

private:
    Animation* m_currentAnimation;
    float m_currentTime{0.0};
    float m_deltaTime{0.0};
    std::vector<glm::mat4> m_finalBoneMatrices{};
};
