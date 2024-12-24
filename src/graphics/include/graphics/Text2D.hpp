#pragma once

#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

#include "Shader.hpp"

class WindowManager;

namespace graphics {

/// @brief 2D text renderer with `Arial` font
/// TODO: Support other font style. Support caching of vertices for rendered characters.
class Text2D {
public:
    explicit Text2D(WindowManager &windowManager);
    Text2D(const Text2D &other) = delete;
    Text2D(Text2D &&other) noexcept = delete;
    Text2D & operator=(const Text2D &other) = delete;
    Text2D & operator=(Text2D &&other) noexcept = delete;

    /// @brief Renders text in screen space coordinates
    ///        TODO: Handle invalid x, y coordinates
    ///
    /// @param text to render
    /// @param x coordinate in screen space
    /// @param y coordinate in screen space
    /// @param scale factor to apply
    /// @param color to apply
    void render(const std::string& text, float x, float y, float scale, const glm::vec3& color);

    /// @brief Renders text in screen space coordinates. Color is based on set value.
    ///
    /// @param text to render
    /// @param x coordinate in screen space
    /// @param y coordinate in screen space
    /// @param scale factor to apply
    void render(const std::string& text, float x, float y, float scale);

    /// @brief Renders text in screen space coordinates. Color and scale is based on set value.
    ///
    /// @param text to render
    /// @param x coordinate in screen space
    /// @param y coordinate in screen space
    void render(const std::string& text, float x, float y);

    /// @param color to apply by default
    void setColor(glm::vec3 color);

    /// @param scale factor to apply by default
    void setScale(float scale);

private:
    void loadCharacterGlpyhs();
    void setupOpenGlBuffers();

    struct Character {
        unsigned int textureId;  // ID handle of the glyph texture
        glm::ivec2   size;       // Size of glyph
        glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
        unsigned int advance;    // Offset to advance to next glyph
    };

    std::unordered_map<char, Character> mCharacters{};
    glm::vec3 mColor{1.0f, 1.0f, 1.0f};
    float mScale{1.0f};
    unsigned int mVAO{};
    unsigned int mVBO{};
    WindowManager& mWindowManager;
    Shader mShader;
};

}