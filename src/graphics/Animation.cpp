#include "Animation.hpp"

Animation::Animation(const std::string &animationFilePath, Model &model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationFilePath, aiProcess_Triangulate);
    assert(scene != nullptr && scene->mRootNode != nullptr);
    auto animation = scene->mAnimations[0];
    m_duration = animation->mDuration;
    m_ticksPerSecond = animation->mTicksPerSecond;
    readHeirarchyData(m_rootNode, scene->mRootNode);
    readMissingBones(animation, model);
}

std::optional<Bone> Animation::findBone(const std::string &name) {
    auto boneIt = std::find_if(m_bones.begin(), m_bones.end(), [&name](const auto& b){
        return b.getBoneName() == name;
    });
    if (boneIt != m_bones.end()) {
        return *boneIt;
    }
    return std::nullopt;
}

float Animation::getTicksPerSecond() {
    return m_ticksPerSecond;
}

float Animation::getDuration() {
    return m_duration;
}

const AssimpNodeData &Animation::getRootNode() {
    return m_rootNode;
}

const std::unordered_map<std::string, Model::BoneInfo> &Animation::getBoneIdMap() {
    return m_boneInfoMap;
}

void Animation::readMissingBones(const aiAnimation *animation, Model &model) {
    auto& boneInfoMap = model.getBoneInfoMap();
    int& boneCount = model.getBoneCount();

    for (auto i = 0; i < animation->mNumChannels; i++) {
        const auto& channel = animation->mChannels[i];
        const std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }

        m_bones.emplace_back(boneName, boneInfoMap[boneName].id, channel);
    }

    m_boneInfoMap = boneInfoMap;
}

void Animation::readHeirarchyData(AssimpNodeData &dest, const aiNode *src) {
    assert(src != nullptr);

    dest.name = src->mName.C_Str();
    dest.transform = [from = src->mTransformation]{
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }();
    dest.childrenCount = src->mNumChildren;

    for (auto i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData{};
        readHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}
