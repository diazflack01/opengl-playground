#include "Mouse.hpp"

#include <fmt/core.h>

void Mouse::setPosition(glm::vec2 pos) {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    if (pos == m_position) {
        return;
    }
    setLastPosition(m_position);
    m_position = pos;
    m_isDragging = m_isLeftButtonPressed || m_isRightButtonPressed;
    fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
}

void Mouse::setScrollDelta(float delta) {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    if (m_scrollDelta == delta) {
        return;
    }
    m_scrollDelta = delta;
    fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
};

void Mouse::endFrame() {
    setScrollDelta(0);
    setLastPosition(m_position);
}

float Mouse::dY() const {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    // y is reversed in screen coordinates
    return m_lastPosition.y - m_position.y;
}

float Mouse::dX() const {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    return m_position.x - m_lastPosition.x;
}

float Mouse::getScrollDelta() const {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    return m_scrollDelta;
}

bool Mouse::isDragging() const {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    return m_isDragging;
}

void Mouse::setLeftButtonPressed(bool pressed) {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    if (m_isLeftButtonPressed == pressed) {
        return;
    }
    m_isLeftButtonPressed = pressed;
    fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
}

void Mouse::setRightButtonPressed(bool pressed) {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    if (m_isRightButtonPressed == pressed) {
        return;
    }
    m_isRightButtonPressed = pressed;
    fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
}

bool Mouse::isLeftButtonPressed() const {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    return m_isLeftButtonPressed;
};

bool Mouse::isRightButtonPressed() const {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    return m_isRightButtonPressed;
}

std::string Mouse::getMouseState() const {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    return fmt::format("MOUSE STATE left button pressed: {}, right button pressed: {}, is dragging: {}, scrollDelta: {}, pos:[{}, {}], last pos:[{}, {}]"
            , m_isLeftButtonPressed, m_isRightButtonPressed, m_isDragging, m_scrollDelta, m_position.x, m_position.y, m_lastPosition.x, m_lastPosition.y);
}

void Mouse::setLastPosition(glm::vec2 pos) {
    std::lock_guard<std::recursive_mutex> lg{m_mutex};
    if (m_lastPosition == pos) {
        return;
    }

    m_lastPosition = pos;
    fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
}