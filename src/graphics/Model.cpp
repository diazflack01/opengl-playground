#include "Model.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <utils/Utils.hpp>
#include <utils/ScopedTimer.hpp>

struct Model::Impl {
    // model data
    std::vector<Mesh> m_meshes;
    std::string m_directory;
    std::vector<Mesh::Texture> m_loadedTextures;
    std::unordered_map<std::string, BoneInfo> m_boneInfoMap;
    int m_boneCounter{0};

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Mesh::Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                    std::string typeName);

    void setVertexBoneDataToDefault(Mesh::Vertex& vertex);
    void setVertexBoneData(Mesh::Vertex& vertex, int boneId, float weight);
    void extractBoneWeightForVertices(std::vector<Mesh::Vertex> &vertices, aiMesh *mesh);
};

Model::Model(const char *path) : m_impl{std::make_unique<Impl>()} {
    m_impl->loadModel(path);
}

void Model::draw(Shader &shader) {
    for (auto idx = 0; idx < m_impl->m_meshes.size(); idx++) {
        m_impl->m_meshes[idx].draw(shader);
    }
}

void Model::drawInstanced(Shader &shader) {
    for (auto& mesh : m_impl->m_meshes) {
        mesh.drawInstanced(shader);
    }
}

void Model::setInstancedModelMatrices(const std::vector<glm::mat4>& modelMatrices) {
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);

    for (auto& mesh : m_impl->m_meshes) {
        mesh.setInstancedModelMatrices(modelMatrices);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::Impl::loadModel(std::string path) {
    ScopedTimer timer{std::string{"loadModel - "} + path};
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    m_directory = path.substr(0, path.find_last_of('/'));

    std::cout << "Root node meshes: " << scene->mRootNode->mNumMeshes << " children: " << scene->mRootNode->mNumChildren << std::endl;

    processNode(scene->mRootNode, scene);
}

void Model::Impl::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        std::cout << "processing mesh: " << i << " numVertices: " << mesh->mNumVertices << "\n";
        m_meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        std::cout << "processing children: " << i << "\n";
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::Impl::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Mesh::Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Mesh::Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex{};

        setVertexBoneDataToDefault(vertex);

        // process vertex positions, normals and texture coordinates
        vertex.position = glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertex.normal = glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->mTextureCoords[0] != nullptr) {
            vertex.texCoords = glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.texCoords = glm::vec2{0.0f, 0.0f};
        }

        vertices.push_back(vertex);
    }

    // process indices
    for (auto idx = 0; idx < mesh->mNumFaces; idx++) {
        const aiFace& face = mesh->mFaces[idx];
        for (auto j = 0; j < face.mNumIndices; j++) {
            indices.emplace_back(face.mIndices[j]);
        }
    }

    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Mesh::Texture> diffuseMaps = loadMaterialTextures(material,
                                                           aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Mesh::Texture> specularMaps = loadMaterialTextures(material,
                                                            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    extractBoneWeightForVertices(vertices, mesh);

    return Mesh{vertices, indices, textures};
}

std::vector<Mesh::Texture> Model::Impl::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Mesh::Texture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        const auto textureFullPathToLoad{m_directory + "/" + str.C_Str()};
        const auto loadedTexture = tryLoadTexture(textureFullPathToLoad);
        if (!loadedTexture.has_value()) {
            std::cerr << "Failed to load texture " << textureFullPathToLoad << "\n";
            continue;
        }

        // find if existing in cache
        const auto cachedTexture = std::find_if(m_loadedTextures.begin(), m_loadedTextures.end(), [&](const auto& t){ return t.path == std::string(str.C_Str()); });
        if (cachedTexture != m_loadedTextures.end()) {
            std::cout << "skip texture loading, already exist in cache id:" << cachedTexture->id << " type:" << cachedTexture->type << " path:" << cachedTexture->path << "\n";
            textures.emplace_back(*cachedTexture);
            continue;
        }

        Mesh::Texture texture{loadedTexture->id, typeName, str.C_Str()};
        std::cout << "adding texture id:" << texture.id << " type:" << texture.type << " path:" << texture.path << "\n";
        m_loadedTextures.emplace_back(texture);
        textures.emplace_back(m_loadedTextures.back());
    }

    return textures;
}

void Model::Impl::setVertexBoneDataToDefault(Mesh::Vertex& vertex) {
    for (auto i = 0; i < MAX_BONE_INFLUENCE; i++) {
        vertex.boneIds[i] = -1;
        vertex.boneWeights[i] = 0.0f;
    }
}

void Model::Impl::setVertexBoneData(Mesh::Vertex& vertex, int boneId, float weight) {
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
        if (vertex.boneIds[i] < 0) {
            vertex.boneWeights[i] = weight;
            vertex.boneIds[i] = boneId;
            break;
        }
    }
}

void Model::Impl::extractBoneWeightForVertices(std::vector<Mesh::Vertex> &vertices, aiMesh *mesh) {
    // find which vertices each bone affects
    for (int boneIdx = 0; boneIdx < mesh->mNumBones; ++boneIdx) {
        // find boneId of given boneName
        int boneId = -1;
        const auto& meshBone = *mesh->mBones[boneIdx];
        const std::string boneName = meshBone.mName.C_Str();
        // use boneInfoMap for caching
        if (m_boneInfoMap.find(boneName) == m_boneInfoMap.end()) {
            BoneInfo newBoneInfo{};
            newBoneInfo.id = m_boneCounter;
            newBoneInfo.offsetMatrix = [from = meshBone.mOffsetMatrix]{
                glm::mat4 to;
                //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
                to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
                to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
                to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
                to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
                return to;
            }();
            m_boneInfoMap[boneName] = newBoneInfo;
            boneId = m_boneCounter;
            m_boneCounter++;
        } else {
            boneId = m_boneInfoMap[boneName].id;
        }

        assert(boneId != -1);

        // update boneId & weight of vertex that's affected by this bone
        const auto weights = meshBone.mWeights;
        const auto numWeights = meshBone.mNumWeights;

        for (int weightIdx = 0; weightIdx < numWeights; ++weightIdx) {
            const int vertexId = weights[weightIdx].mVertexId;
            const float weight = weights[weightIdx].mWeight;
            assert(vertexId <= vertices.size());
            setVertexBoneData(vertices[vertexId], boneId, weight);
        }
    }
}

std::unordered_map<std::string, Model::BoneInfo> &Model::getBoneInfoMap() {
    return m_impl->m_boneInfoMap;
}

int & Model::getBoneCount() {
    return m_impl->m_boneCounter;
}

Model::Model(Model &&other) noexcept {
    m_impl = std::move(other.m_impl);
}

Model::~Model() {}


Transform::Transform(glm::vec3 positionIn, glm::vec3 rotationIn, glm::vec3 scaleIn)
 : position{positionIn}, rotation{rotationIn}, scale{scaleIn}
{}

glm::mat4 Transform::getModelMatrix() const {
    const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
                         glm::radians(rotation.x),
                         glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
                         glm::radians(rotation.y),
                         glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
                         glm::radians(rotation.z),
                         glm::vec3(0.0f, 0.0f, 1.0f));

    // Y * X * Z
    const glm::mat4 roationMatrix = transformY * transformX * transformZ;

    // translation * rotation * scale (also know as TRS matrix)
    return glm::translate(glm::mat4(1.0f), position) *
                roationMatrix *
                glm::scale(glm::mat4(1.0f), scale);
}

std::shared_ptr<SceneNode> SceneNode::addChild(std::shared_ptr<SceneNode> node) {
    mChildren.push_back(node);
    node->mParent = shared_from_this();
    return node;
}

void SceneNode::setParent(std::shared_ptr<SceneNode> parent) {
    mParent = std::move(parent);
}

void SceneNode::updateSelfAndChild() {
    if (mIsDirty) {
        updateSelfAndChildForced();
        return;
    }

    for (auto& child : mChildren) {
        child->updateSelfAndChild();
    }
}

void SceneNode::updateSelfAndChildForced() {
    calcModelMatrix();
    mIsDirty = false;

    for (auto& child : mChildren) {
        child->updateSelfAndChildForced();
    }
}

void SceneNode::setPosition(glm::vec3 position) {
    mTransform.position = position;
    mIsDirty = true;
}
void SceneNode::setRotation(glm::vec3 rotation) {
    mTransform.rotation = rotation;
    mIsDirty = true;
}
void SceneNode::setScale(glm::vec3 scale) {
    mTransform.scale = scale;
    mIsDirty = true;
}
void SceneNode::setScale(float scale) {
    mTransform.scale = glm::vec3{scale, scale, scale};
    mIsDirty = true;
}

glm::mat4 SceneNode::getModelMatrix() {
    return mModelMatrix;
}

std::vector<std::shared_ptr<SceneNode>> SceneNode::getChildren() const {
    return mChildren;
}

void SceneNode::calcModelMatrix() {
    if (mParent) {
        mModelMatrix = mParent->mModelMatrix * mTransform.getModelMatrix();
    } else {
        mModelMatrix = mTransform.getModelMatrix();
    }
}

MeshSceneNode::MeshSceneNode(Model model)
 : mModel{std::move(model)}
{}

void MeshSceneNode::draw(Shader &shader) {
    shader.use();

    shader.setMat4("model", getModelMatrix());
    mModel.draw(shader);
}
