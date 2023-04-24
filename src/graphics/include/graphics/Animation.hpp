#pragma once

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

#include <glm/mat4x4.hpp>

#include <assimp/anim.h>
#include <assimp/scene.h>

#include "Model.hpp"
#include "Bone.hpp"

struct AssimpNodeData {
    glm::mat4 transform;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation {
public:
    Animation(const std::string& animationFilePath, Model& model);

    std::optional<Bone> findBone(const std::string& name);

    float getTicksPerSecond();

    float getDuration();

    const AssimpNodeData& getRootNode();

    const std::unordered_map<std::string, Model::BoneInfo>& getBoneIdMap();

private:
    void readMissingBones(const aiAnimation* animation, Model& model);

    void readHeirarchyData(AssimpNodeData& dest, const aiNode* src);

    double m_duration;
    double m_ticksPerSecond;
    std::vector<Bone> m_bones;
    AssimpNodeData m_rootNode;
    std::unordered_map<std::string, Model::BoneInfo> m_boneInfoMap;
};
