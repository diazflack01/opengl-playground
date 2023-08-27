#pragma once

#include <utils/Utils.hpp>

class Texture {
public:
    Texture(const std::string& texturePath, int defaultTextureUnit, const TextureLoadConfig &textureLoadConfig = {});

    void bind();
    void bind(int textureUnit);

    void unbind();

private:
    int m_defaultTextureUnit;
    int m_lastBoundedTextureUnit{0};
    TextureContext m_textureContext{};
};