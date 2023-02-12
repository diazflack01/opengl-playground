#pragma once

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>

#include "Mesh.hpp"
#include "Shader.hpp"
#include "glm/fwd.hpp"

class Model {
public:
    explicit Model(const char* path);

    void draw(Shader &shader);

    void setInstancedModelMatrices(const std::vector<glm::mat4>& modelMatrices);

    void drawInstanced(Shader &shader);

private:
    // model data
    std::vector<Mesh> mMeshes;
    std::string mDirectory;
    std::vector<Mesh::Texture> mLoadedTextures;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Mesh::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                         std::string typeName);
};

struct Transform {
    Transform(glm::vec3 positionIn, glm::vec3 rotationIn, glm::vec3 scaleIn);
    Transform() = default;
    Transform(const Transform& o) = default;
    Transform(Transform&& o) = default;
    Transform& operator=(const Transform& o) = default;
    Transform& operator=(Transform&& o) = default;
    ~Transform() = default;

    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    glm::mat4 getModelMatrix() const;
};

class IDrawable {
public:
    virtual void draw(Shader& shader) = 0;
    virtual ~IDrawable() = default;
};

class SceneNode : public std::enable_shared_from_this<SceneNode> {
public:
    virtual ~SceneNode() = default;

    std::shared_ptr<SceneNode> addChild(std::shared_ptr<SceneNode> node);

    void setParent(std::shared_ptr<SceneNode> parent);
    
    void updateSelfAndChild();

    void updateSelfAndChildForced();

    // local transform
    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scale);
    void setScale(float scale);

    glm::mat4 getModelMatrix();

    std::vector<std::shared_ptr<SceneNode>> getChildren() const;

protected:
    std::shared_ptr<SceneNode> mParent{nullptr};
    std::vector<std::shared_ptr<SceneNode>> mChildren{};
    Transform mTransform{}; // local space
    glm::mat4 mModelMatrix{1.0f}; // world space, takes into account parent model matrix
    bool mIsDirty{true};

    void calcModelMatrix();
};

class MeshSceneNode : public SceneNode, public IDrawable {
public:
    explicit MeshSceneNode(Model model);
    MeshSceneNode() = delete;
    MeshSceneNode(const MeshSceneNode& o) = default;
    MeshSceneNode(MeshSceneNode&& o) = default;
    MeshSceneNode& operator=(const MeshSceneNode& o) = default;
    MeshSceneNode& operator=(MeshSceneNode&& o) = default;
    ~MeshSceneNode() = default;

    void draw(Shader& shader) override;

private:
    Model mModel;
};
