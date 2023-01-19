#include "Utils.hpp"

#include <glad/glad.h>
#include <iostream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.hpp>

std::optional<TextureContext> tryLoadTexture(const std::string& texturePath, TextureLoadConfig textureLoadConfig)
{
    stbi_set_flip_vertically_on_load(textureLoadConfig.flipVertically);

    unsigned int id;
    glGenTextures(1, &id);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrComponents, 0);
    if (data != nullptr) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        const auto textureName = texturePath.substr(texturePath.find_last_of('/') + 1, texturePath.size());

        std::cout << "tryLoadTexture - '" << textureName << "' width: " << width << " height: " << height << " components: " << nrComponents << "\n";
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureLoadConfig.wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureLoadConfig.wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureLoadConfig.minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureLoadConfig.magFilter);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture '" << texturePath << "'\n";
        return std::nullopt;
    }

    return TextureContext{id, width, height, nrComponents};
}

unsigned loadCubemapTexture(std::vector<std::string> faces, TextureLoadConfig textureLoadConfig) {
    stbi_set_flip_vertically_on_load(textureLoadConfig.flipVertically);

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    int width, height, nrComponents;

    for (auto i = 0u; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data != nullptr) {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            const auto textureName = faces[i].substr(faces[i].find_last_of('/') + 1, faces[i].size());

            std::cout << "loadCubemapTexture - '" << textureName << "' width: " << width << " height: " << height << " components: " << nrComponents << "\n";
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "loadCubemapTexture - failed to load texture '" << faces[i] << "'\n";
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, textureLoadConfig.wrapS);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, textureLoadConfig.wrapT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, textureLoadConfig.wrapR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, textureLoadConfig.minFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, textureLoadConfig.magFilter);

    return id;
}
