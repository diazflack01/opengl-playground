#pragma once

class WindowManager;

namespace graphics {

/// @brief Hardcoded to support GLFW and OpenGL 3. Can be sub-classed
///        for custom visualization by overriding `update` method
class ImGuiWrapper {
public:
    /// @param windowManager window where this ImGui will be contained
    explicit ImGuiWrapper(WindowManager &windowManager);
    ImGuiWrapper(const ImGuiWrapper &other) = delete;
    ImGuiWrapper(ImGuiWrapper &&other) noexcept = delete;
    ImGuiWrapper & operator=(const ImGuiWrapper &other) = delete;
    ImGuiWrapper & operator=(ImGuiWrapper &&other) noexcept = delete;
    virtual ~ImGuiWrapper() = default;

    /// Main method to be called in application's main loop
    void render();

private:
    /// ImGui widget generation, can be overridden by child class for custom visualization
    virtual void update();

    /// updates window manager state after every ImGui frame update
    void updateWindowManager() const;

    /// Window Manager states
    bool mShowDemoWindow{false};
    bool mEnableVSync{true};
    bool mHideCursor{false};
    bool mDrawWireFrame{false};
    bool mEnableDepthTesting{true};

    WindowManager& mWindowManager;
};

}