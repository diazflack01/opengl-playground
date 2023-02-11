#pragma once

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"
#include "Shader.hpp"
#include "glm/fwd.hpp"

class Model {
public:
    Model(const char* path);

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
