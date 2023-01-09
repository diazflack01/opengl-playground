#include "Utils.hpp"

#include <glad/glad.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.hpp>

std::optional<TextureContext> tryLoadTexture(const std::string& texturePath, bool flipVertically)
{
    stbi_set_flip_vertically_on_load(flipVertically);

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

        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture '" << texturePath << "'\n";
        return std::nullopt;
    }
    stbi_image_free(data);

    return TextureContext{id, width, height, nrComponents};
}