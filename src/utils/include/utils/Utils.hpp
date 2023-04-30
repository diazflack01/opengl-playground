#pragma once

#include <string>
#include <optional>
#include <glad/glad.h>
#include <vector>

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
    unsigned wrapR = GL_REPEAT;
    unsigned minFilter = GL_LINEAR_MIPMAP_LINEAR;
    unsigned magFilter = GL_LINEAR;
};

std::optional<TextureContext> tryLoadTexture(const std::string& texturePath, TextureLoadConfig textureLoadConfig = {});
TextureContext loadTexture(const std::string& texturePath, TextureLoadConfig textureLoadConfig = {});

unsigned loadCubemapTexture(std::vector<std::string> faces,
                            TextureLoadConfig textureLoadConfig = TextureLoadConfig{false, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR});