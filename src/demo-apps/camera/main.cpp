#include <iostream>
#include <vector>
#include <mutex>

#include <fmt/core.h>

#include <graphics/WindowManager.hpp>
#include <graphics/Shader.hpp>
#include <utils/Utils.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp> // glm::to_string

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGTH = 600.0f;
const auto WINDOW_TITLE = APP_NAME;
constexpr glm::mat4 IDENTITY_MATRIX{1.0};

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// mouse position
float lastX = SCREEN_WIDTH / 2.0;
float lastY = SCREEN_HEIGTH / 2.0;
bool firstMouse = true;
float pitch = 0; // positive: look up, negative: look down
float yaw = -90; // positive: rotate right, negative: rotate left
float fov = 45.0f; // smaller: zoomed-in, larger: zoomed-out

void processKeyboardInput(GLFWwindow* window);
void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

class Mouse {
public:
    void setPosition(glm::vec2 pos) {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        if (pos == m_position) {
            return;
        }
        setLastPosition(m_position);
        m_position = pos;
        m_isDragging = m_isLeftButtonPressed || m_isRightButtonPressed;
        fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
    }

    void setScrollDelta(float delta) {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        if (m_scrollDelta == delta) {
            return;
        }
        m_scrollDelta = delta;
        fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
    };

    void endFrame() {
        setScrollDelta(0);
        setLastPosition(m_position);
    }

    float dY() const {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        // y is reversed in screen coordinates
        return m_lastPosition.y - m_position.y;
    }

    float dX() const {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        return m_position.x - m_lastPosition.x;
    }

    float getScrollDelta() const {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        return m_scrollDelta;
    }

    bool isDragging() const {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        return m_isDragging;
    }

    void setLeftButtonPressed(bool pressed) {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        if (m_isLeftButtonPressed == pressed) {
            return;
        }
        m_isLeftButtonPressed = pressed;
        fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
    }

    void setRightButtonPressed(bool pressed) {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        if (m_isRightButtonPressed == pressed) {
            return;
        }
        m_isRightButtonPressed = pressed;
        fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
    }

    bool isLeftButtonPressed() const {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        return m_isLeftButtonPressed;
    };

    bool isRightButtonPressed() const {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        return m_isRightButtonPressed;
    }

    std::string getMouseState() const {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        return fmt::format("MOUSE STATE left button pressed: {}, right button pressed: {}, is dragging: {}, scrollDelta: {}, pos:[{}, {}], last pos:[{}, {}]"
        , m_isLeftButtonPressed, m_isRightButtonPressed, m_isDragging, m_scrollDelta, m_position.x, m_position.y, m_lastPosition.x, m_lastPosition.y);
    }

private:
    void setLastPosition(glm::vec2 pos) {
        std::lock_guard<std::recursive_mutex> lg{m_mutex};
        if (m_lastPosition == pos) {
            return;
        }

        m_lastPosition = pos;
        fmt::println("{} -- {}", getMouseState(), __FUNCTION__);
    }

    bool m_isLeftButtonPressed{false};
    bool m_isRightButtonPressed{false};
    bool m_isDragging{false};
    float m_scrollDelta{0};
    glm::vec2 m_position{0,0};
    glm::vec2 m_lastPosition{0,0};
    mutable std::recursive_mutex m_mutex{};
};

Mouse mouse;

int main(int argc, char** argv) {
    fmt::println("main ()");

    WindowManager windowManager{SCREEN_WIDTH, SCREEN_HEIGTH, WINDOW_TITLE};

    // mouse move and scroll callbacks
    glfwSetCursorPosCallback(windowManager.getWindow(), mouseMoveCallback);
    glfwSetScrollCallback(windowManager.getWindow(), mouseScrollCallback);
    glfwSetMouseButtonCallback(windowManager.getWindow(), mouseButtonCallback);

    // disable mouse cursor
//    glfwSetInputMode(windowManager.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // vertices will be used in NDC which is (-1,-1) bottom-left to (1,1) top-right with origin at (0,0)
    float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    const std::vector<glm::vec3> cubePositions{
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // create VAO, VBO, EBO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // bind VAO first, so all VBO, EBO buffer calls will be recorded
    glBindVertexArray(VAO);
    // bind buffer & set data - VBO, EBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set VAO data layout attributes for vertex shader usage
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind VAO
    glBindVertexArray(0);

    Shader shader{"resources/shader/demo_coordinate_system.vert", "resources/shader/demo_coordinate_system.frag"};
    const auto textureAwesomeFace = loadTexture("resources/texture/awesomeface.png");
    const auto textureWoodenContainer = loadTexture("resources/texture/wooden_container.jpg");
    const auto texturePepefrog = loadTexture("resources/texture/pepe-frog.jpg");

    // draw wireframe. Default is GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // enable depth/z-buffer testing
    glEnable(GL_DEPTH_TEST);

    while (!windowManager.isCloseRequested()) {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        windowManager.update();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use program
        shader.use();

        // activate and set texture unit
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureAwesomeFace.id);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, texturePepefrog.id);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, textureWoodenContainer.id);

        // set texture sampler2D's in fragment shader
        shader.setInt("texture0", 4); // 4 - GL_TEXTURE4
        shader.setInt("texture1", 8); // 8 - GL_TEXTURE8

        // camera
        processKeyboardInput(windowManager.getWindow());
        const auto view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // bind VAO then draw the cube
        glBindVertexArray(VAO);

        // MVP
        const auto projection = glm::perspective(glm::radians(fov), SCREEN_WIDTH/SCREEN_HEIGTH, 0.1f, 100.0f);

        // set uniform values
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // draw cubes with different model matrix
        for (auto idx = 0; idx < cubePositions.size(); idx++) {
            shader.setInt("texture1", idx % 2 == 0 ? 0 : 8); // change textures

            auto model = glm::translate(IDENTITY_MATRIX, cubePositions[idx]);
            const float angle = 20.0f * idx;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        windowManager.swapBuffers();
        mouse.endFrame();
    }

    glfwTerminate();
    return 0;
}

void processKeyboardInput(GLFWwindow* window) {
    const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        // cross (front, up) -> generates cam right vector
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    // screen-space coordinate goes from top to bottom, so subtraction is flipped to get positive number when mouse is
    // moved upwards, and negative when moved downwards. This is necessary because pitch calculation uses negative
    // number to look down and positive to look up
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    mouse.setPosition(glm::vec2{xpos, ypos});

    /*** Raycast from mouse click ***/
    // screen-space/viewport to normalized device coordinates space
    const float ndc_x = (2.0f * xpos) / SCREEN_WIDTH - 1.0f;
    const float ndc_y = 1.0f - (2.0f * ypos) / SCREEN_HEIGTH;
    fmt::println("viewport:[{}, {}] NDC:[{}, {}]", xpos, ypos, ndc_x, ndc_y);

    // NDC to homogenous clip coordinates by adding `w` component.
    // `z` component is assigned -1.0f since we're casting a ray
    const glm::vec4 rayHomogenousClipSpace{ndc_x, ndc_y, -1.0f, 1.0f};
    fmt::println("Ray Homogenous Clip Space: [{}]", glm::to_string(rayHomogenousClipSpace));

    // Homogenous clip space to eye/camera space coordinates
    const auto projection = glm::perspective(glm::radians(fov), SCREEN_WIDTH/SCREEN_HEIGTH, 0.1f, 100.0f);
    const auto rayCameraSpace = glm::inverse(projection) * rayHomogenousClipSpace;
    fmt::println("Ray Eye/Camera Space: [{}]", glm::to_string(rayCameraSpace));

    // Eye/camera space to world space coordinates
    const auto view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    const auto rayWorldSpaceVec4 = (glm::inverse(view) * rayCameraSpace);
    const glm::vec3 rayWorldSpaceNormalized{rayWorldSpaceVec4.x, rayWorldSpaceVec4.y, rayWorldSpaceVec4.z};
    fmt::println("Ray World Space: [{}]", glm::to_string(rayWorldSpaceNormalized));
}

void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;

    mouse.setScrollDelta(yoffset);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            const auto leftButtonPressed = action == GLFW_PRESS;
            mouse.setLeftButtonPressed(leftButtonPressed);
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            const auto rightButtonPressed = action == GLFW_PRESS;
            mouse.setRightButtonPressed(rightButtonPressed);
            break;
        }
    }
}