#pragma once

#include <string>
#include <memory>

#include <glad/glad.h> // needs to be included before glfw
#include <GLFW/glfw3.h>

class WindowManager {
public:
    WindowManager(int width, int height, std::string title);

    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    WindowManager(WindowManager&&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;
    ~WindowManager();

    void update();

    [[nodiscard]] bool isCloseRequested() const;

    void swapBuffers();

    void setWidth(int width);
    void setHeight(int height);
    void setTitle(const std::string& title);

    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] const std::string &getTitle() const;

    GLFWwindow* getWindow();

    // enable/disable vsync, this wont have an effect if vsync
    // is enabled in GPU driver setting
    void setVSyncEnabled(bool enable);
    bool isVSyncEnabled() const;

private:
    static void processKeyboardInputs(GLFWwindow* window);
    void updateWindowSize();

    int m_width;
    int m_height;
    std::string m_title;
    GLFWwindow* m_window{nullptr};
    bool m_isCloseRequested{false};
    bool m_enableVSync{true};
};