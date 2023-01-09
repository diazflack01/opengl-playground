#pragma once

#include <string>
#include <optional>

struct TextureContext
{
    unsigned int id;
    int width;
    int height;
    int numComponents;
};

std::optional<TextureContext> tryLoadTexture(const std::string& texturePath, bool flipVertically = true);