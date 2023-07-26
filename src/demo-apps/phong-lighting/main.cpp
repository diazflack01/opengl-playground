#include <iostream>
#include <vector>
#include <mutex>

#include <fmt/core.h>

#include <graphics/WindowManager.hpp>
#include <graphics/Shader.hpp>
#include <graphics/Mouse.hpp>
#include <graphics/Camera.hpp>

#include <utils/Utils.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp> // glm::to_string

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGTH = 600.0f;
const auto WINDOW_TITLE = "demo-phong-lighting";
constexpr glm::mat4 IDENTITY_MATRIX{1.0f};

const glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  5.0f);
const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// mouse position
const glm::vec2 initialXYPos{SCREEN_WIDTH / 2.0, SCREEN_HEIGTH / 2.0};
const float pitch = 0; // positive: look up, negative: look down
const float yaw = -90; // positive: rotate right, negative: rotate left
const float fov = 45.0f; // smaller: zoomed-in, larger: zoomed-out

const float aswdMovementSensitivity = 2.5f;
const float mouseMovementSensitivity = 0.1f;

void processKeyboardInput(GLFWwindow* window);
void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

const Camera::ConfigState camConfig{cameraPos,
                                    cameraFront,
                                    cameraUp,
                              Camera::BoundedData<float>{fov, 1, 45},
                              Camera::BoundedData<float>{pitch, -89, 89},
                              Camera::BoundedData<float>{yaw, std::nullopt, std::nullopt},
                              Camera::Sensitivity{aswdMovementSensitivity, mouseMovementSensitivity},
                                    initialXYPos,
                                    true};
Camera camera{camConfig};

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
    // pos (px,py,pz), normal (nx,ny,nz)
    float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind VAO
    glBindVertexArray(0);

    Shader phongLightingShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/demo_phong_lighting.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/demo_phong_lighting.frag"};
    Shader lightCubeShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/demo_phong_lighting_light.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/demo_phong_lighting_light.frag"};

    // light cube position data
    const glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
    const auto lightCubeScaleMatrix = glm::scale(IDENTITY_MATRIX, glm::vec3{0.2f});
    const auto lightCubeTranslateMatrix = glm::translate(IDENTITY_MATRIX, lightPos);

    // draw wireframe. Default is GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // enable depth/z-buffer testing
    glEnable(GL_DEPTH_TEST);

    while (!windowManager.isCloseRequested()) {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // call update to poll events of window
        windowManager.update();

        // process keyboard inputs
        processKeyboardInput(windowManager.getWindow());

        // clear buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view and projection matrix
        const auto view = camera.getViewMatrix();
        const auto projection = glm::perspective(glm::radians(camera.getFieldOfView()), SCREEN_WIDTH/SCREEN_HEIGTH, 0.1f, 100.0f);

        // bind cube VAO
        glBindVertexArray(VAO);

        // draw light cube
        lightCubeShader.use();
        lightCubeShader.setMat4("model", lightCubeTranslateMatrix * lightCubeScaleMatrix);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw cube with phong lighting
        phongLightingShader.use();
        const auto& camPos = camera.getPosition();
        phongLightingShader.setVec3Float("viewPos", camPos.x, camPos.y, camPos.z);
        phongLightingShader.setVec3Float("lightPos", lightPos.x, lightPos.y, lightPos.z);
        phongLightingShader.setVec3Float("lightColor", 1.0f, 1.0f, 1.0f);
        phongLightingShader.setVec3Float("objectColor", 1.0f, 0.5f, 0.31f);
        phongLightingShader.setMat4("model", IDENTITY_MATRIX);
        phongLightingShader.setMat4("view", view);
        phongLightingShader.setMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        windowManager.swapBuffers();
        mouse.endFrame();
    }

    glfwTerminate();
    return 0;
}

void processKeyboardInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processMovement(Camera::Movement::Forward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processMovement(Camera::Movement::Backward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processMovement(Camera::Movement::Left, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processMovement(Camera::Movement::Right, deltaTime);
}

void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    camera.processMouseMovement(xpos, ypos);
    mouse.setPosition(glm::vec2{xpos, ypos});
}

void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(yoffset);
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