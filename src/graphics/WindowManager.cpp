#include <WindowManager.hpp>

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

WindowManager::WindowManager(int width, int height, std::string title)
 : m_width{width}
 , m_height{height}
 , m_title{std::move(title)} {

    if (glfwInit() == GLFW_FALSE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, m_title.c_str(), nullptr, nullptr);

    if (m_window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, m_width, m_height);
}

WindowManager::~WindowManager() {
    glfwTerminate();
}

void WindowManager::update() {
    glfwPollEvents();

    processKeyboardInputs(m_window);

    m_isCloseRequested = glfwWindowShouldClose(m_window);
}

bool WindowManager::isCloseRequested() const {
    return m_isCloseRequested;
}

void WindowManager::setWidth(int width) {
    m_width = width;
    updateWindowSize();
}

void WindowManager::setHeight(int height) {
    m_height = height;
    updateWindowSize();
}

void WindowManager::setTitle(const std::string &title) {
    m_title = title;
    glfwSetWindowTitle(m_window, m_title.c_str());
}

int WindowManager::getWidth() const {
    return m_width;
}

int WindowManager::getHeight() const {
    return m_height;
}

const std::string &WindowManager::getTitle() const {
    return m_title;
}

void WindowManager::processKeyboardInputs(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void WindowManager::swapBuffers() {
    glfwSwapBuffers(m_window);
}

void WindowManager::updateWindowSize() {
    glfwSetWindowSize(m_window, m_width, m_height);
    glViewport(0, 0, m_width, m_height);
}

GLFWwindow *WindowManager::getWindow() {
    return m_window;
}
