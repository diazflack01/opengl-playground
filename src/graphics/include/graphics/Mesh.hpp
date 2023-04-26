#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "glm/fwd.hpp"

constexpr int MAX_BONE_INFLUENCE = 4;

class Mesh {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;

        // bone idx that will influence this vertex
        int boneIds[MAX_BONE_INFLUENCE];
        // weights from each bone
        float boneWeights[MAX_BONE_INFLUENCE];
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
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;

    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;

    std::optional<int> m_instanceCount;

    void setupMesh();
    void bindTextures(Shader& shader);
};
