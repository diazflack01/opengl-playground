#pragma once

#include <mutex>

#include <glm/vec2.hpp>

class Mouse {
public:
    void setPosition(glm::vec2 pos);

    void setScrollDelta(float delta);

    /*!
     * Must be called at the end of frame-loop
     */
    void endFrame();

    float dY() const;

    float dX() const;

    float getScrollDelta() const;

    bool isDragging() const;

    void setLeftButtonPressed(bool pressed);

    void setRightButtonPressed(bool pressed);

    bool isLeftButtonPressed() const;

    bool isRightButtonPressed() const;

    std::string getMouseState() const;

private:
    void setLastPosition(glm::vec2 pos);

    bool m_isLeftButtonPressed{false};
    bool m_isRightButtonPressed{false};
    bool m_isDragging{false};
    float m_scrollDelta{0};
    glm::vec2 m_position{0,0};
    glm::vec2 m_lastPosition{0,0};
    mutable std::recursive_mutex m_mutex{};
};