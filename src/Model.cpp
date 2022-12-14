#include "Model.hpp"

#include <iostream>

#include "Utils.hpp"
#include "ScopedTimer.hpp"

Model::Model(const char *path) {
    loadModel(path);
}

void Model::draw(Shader &shader) {
    for (auto idx = 0; idx < mMeshes.size(); idx++) {
        mMeshes[idx].draw(shader);
    }
}

void Model::loadModel(std::string path) {
    ScopedTimer timer{std::string{"loadModel - "} + path};
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    mDirectory = path.substr(0, path.find_last_of('/'));

    std::cout << "Root node meshes: " << scene->mRootNode->mNumMeshes << " children: " << scene->mRootNode->mNumChildren << std::endl;

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        std::cout << "processing mesh: " << i << "\n";
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        mMeshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        std::cout << "processing children: " << i << "\n";
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Mesh::Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Mesh::Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;
        // process vertex positions, normals and texture coordinates
        vertex.Position = glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertex.Normal = glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->mTextureCoords[0] != nullptr) {
            vertex.TexCoords = glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        } else {
            vertex.TexCoords = glm::vec2{0.0f, 0.0f};
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

    return Mesh{vertices, indices, textures};
}

std::vector<Mesh::Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Mesh::Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip{false};

        const auto textureFullPathToLoad{mDirectory + "/" + str.C_Str()};
        const auto loadedTexture = tryLoadTexture(textureFullPathToLoad);
        if (!loadedTexture.has_value()) {
            std::cerr << "Failed to load texture " << textureFullPathToLoad << "\n";
            continue;
        }

        // find if existing in cache
        const auto cachedTexture = std::find_if(mLoadedTextures.begin(), mLoadedTextures.end(), [&](const auto& t){ return t.path == std::string(str.C_Str()); });
        if (cachedTexture != mLoadedTextures.end()) {
            skip = true;
            std::cout << "skip texture loading, already exist in cache id:" << cachedTexture->id << " type:" << cachedTexture->type << " path:" << cachedTexture->path << "\n";
            textures.emplace_back(*cachedTexture);
        }


        if (!skip) {
            Mesh::Texture texture{loadedTexture->id, typeName, str.C_Str()};
            std::cout << "adding texture id:" << texture.id << " type:" << texture.type << " path:" << texture.path << "\n";
            mLoadedTextures.emplace_back(texture);
            textures.emplace_back(mLoadedTextures.back());
        }
    }

    return textures;
}
