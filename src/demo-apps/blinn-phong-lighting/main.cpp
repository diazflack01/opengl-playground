#include "glm/ext/matrix_transform.hpp"
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
const auto WINDOW_TITLE = "demo-blinn-phong-lighting";
constexpr glm::mat4 IDENTITY_MATRIX{1.0f};

const glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
const glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool useBlinnPhong = false;
bool useBlinnKeyPressed = false;
bool useMaterialSpecular = false;
bool useMaterialSpecularKeyPressed = false;

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
    glfwSetInputMode(windowManager.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    Shader blinnPhongLightingShader{"resources/shader/demo_phong_lighting.vert", "resources/shader/demo_blinn_phong_lighting.frag"};
    const auto containerDiffuse = loadTexture("resources/texture/container2.png");
    const auto containerSpecular = loadTexture("resources/texture/container2_specular.png");
    const auto floorTexture = loadTexture("resources/texture/wood.png");

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

        // draw plane
        blinnPhongLightingShader.use();
        blinnPhongLightingShader.setBool("useBlinn", useBlinnPhong);
        blinnPhongLightingShader.setBool("useMaterialSpecular", useMaterialSpecular);
        // mvp
        blinnPhongLightingShader.setMat4("view", view);
        blinnPhongLightingShader.setMat4("projection", projection);
        blinnPhongLightingShader.setMat4("model", IDENTITY_MATRIX);
        const auto& camPos = camera.getPosition();
        blinnPhongLightingShader.setVec3Float("viewPos", camPos.x, camPos.y, camPos.z);
        // light
        const glm::vec3 lightPos{.0f, .0f, .0f};
        blinnPhongLightingShader.setVec3Float("light.position",  lightPos.x, lightPos.y, lightPos.z);
        blinnPhongLightingShader.setVec3Float("light.ambient",  0.05f, 0.05f, 0.05f);
        blinnPhongLightingShader.setVec3Float("light.diffuse",  1.0f, 1.0f, 1.0f);
        // weaken the specular to make difference of phong vs blinn obvious
        blinnPhongLightingShader.setVec3Float("light.specular", .3f, .3f, .3f);

        // material properties
        // shininess - very low to make difference of phong vs blinn obvious
        blinnPhongLightingShader.setFloat("material.shininess", 0.5f);
        // textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, containerDiffuse.id);
        blinnPhongLightingShader.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, containerSpecular.id);
        blinnPhongLightingShader.setInt("material.specular", 1);

        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !useBlinnKeyPressed) {
        useBlinnKeyPressed = true;
        useBlinnPhong = !useBlinnPhong;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
        useBlinnKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !useMaterialSpecularKeyPressed) {
        useMaterialSpecularKeyPressed = true;
        useMaterialSpecular = !useMaterialSpecular;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
        useMaterialSpecularKeyPressed = false;
    }
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