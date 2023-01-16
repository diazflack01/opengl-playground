#pragma once

#include <string>
#include <optional>
#include <glad/glad.h>

struct TextureContext
{
    unsigned int id;
    int width;
    int height;
    int numComponents;
};

struct TextureLoadConfig
{
    bool flipVertically = true;
    unsigned wrapS = GL_REPEAT;
    unsigned wrapT = GL_REPEAT;
    unsigned minFilter = GL_LINEAR_MIPMAP_LINEAR;
    unsigned magFilter = GL_LINEAR;
};

std::optional<TextureContext> tryLoadTexture(const std::string& texturePath, TextureLoadConfig textureLoadConfig = {});
