#include "Text2D.hpp"

#include "WindowManager.hpp"
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/ext/matrix_clip_space.hpp>



namespace graphics {
Text2D::Text2D(WindowManager &windowManager) : mWindowManager{windowManager}, mShader{"resources/shader/text_2d.vert", "resources/shader/text_2d.frag"} {
    loadCharacterGlpyhs();
    setupOpenGlBuffers();
}

void Text2D::render(const std::string& text, float x, float y, float scale, const glm::vec3 &color) {
    mShader.use();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const auto projection = glm::ortho(0.0f, static_cast<float>(mWindowManager.getWidth()), 0.0f, static_cast<float>(mWindowManager.getHeight()));
    mShader.setMat4("projection", projection);
    mShader.setVec3Float("textColor", color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(mVAO);

    // iterate through all characters
    for (auto it = text.cbegin(); it != text.cend(); it++)
    {
        const Character& ch = mCharacters[*it];

        const float xpos = x + ch.bearing.x * scale;
        const float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        const float w = ch.size.x * scale;
        const float h = ch.size.y * scale;

        // update VBO for each character
        // NOTE: Loaded texture is flipped where top-left is (0, 0), bottom-right (1, 1)
        const float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureId);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Text2D::render(const std::string& text, float x, float y, float scale) {

    render(text, x, y, scale, mColor);
}

void Text2D::render(const std::string& text, float x, float y) {
    render(text, x, y, mScale, mColor);
}

void Text2D::setColor(glm::vec3 color) {
    mColor = color;
}

void Text2D::setScale(float scale) {
    mScale = scale;
}

void Text2D::loadCharacterGlpyhs() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "resources/fonts/Arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character{
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        mCharacters.insert(std::pair<char, Character>(c, character));
    }

    // release resources
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void Text2D::setupOpenGlBuffers() {
    // allocate, bind, set initial data
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    constexpr auto verticesCnt = 6;
    constexpr auto vertexDimensionCnt = 4;
    constexpr auto oneDimensionSize = sizeof(float);
    constexpr auto vertexSize = oneDimensionSize * verticesCnt * vertexDimensionCnt;
    glBufferData(GL_ARRAY_BUFFER, vertexSize, NULL, GL_DYNAMIC_DRAW);

    // setup expected shader input
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertexDimensionCnt * oneDimensionSize, 0);

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
}
