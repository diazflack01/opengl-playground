#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "glm/fwd.hpp"

class Mesh {
public:
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void draw(Shader& shader);

    void setInstancedModelMatrices(const std::vector<glm::mat4>& modelMatrices);

    void drawInstanced(Shader& shader);

private:
    std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<Texture> mTextures;

    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int mEBO;

    std::optional<int> mInstanceCount;

    void setupMesh();
    void bindTextures(Shader& shader);
};
