#include <Texture.hpp>

#include <fmt/core.h>

Texture::Texture(const std::string &texturePath, int defaultTextureUnit, const TextureLoadConfig &textureLoadConfig)
    : m_defaultTextureUnit{defaultTextureUnit}
{
    auto textureContext = tryLoadTexture(texturePath, textureLoadConfig);
    if (textureContext == std::nullopt) {
        throw std::runtime_error(fmt::format("Failed to load texture, texturePath:{}", texturePath));
    }

    m_textureContext = std::move(textureContext.value());
}

void Texture::bind() {
    glActiveTexture(GL_TEXTURE0 + m_defaultTextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureContext.id);
    m_lastBoundedTextureUnit = m_defaultTextureUnit;
}

void Texture::bind(int textureUnit) {
    if (textureUnit < 0 || textureUnit > 31) {
        throw std::logic_error{fmt::format("Texture unit {} invalid! Valid values range is 0-31", textureUnit)};
    }

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureContext.id);
    m_lastBoundedTextureUnit = textureUnit;
}

void Texture::unbind() {
    if (m_lastBoundedTextureUnit == 0) {
        return;
    }
    glActiveTexture(GL_TEXTURE0 + m_lastBoundedTextureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_lastBoundedTextureUnit = 0;
}
