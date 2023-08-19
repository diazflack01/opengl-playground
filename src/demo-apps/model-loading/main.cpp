#include <iostream>
#include <optional>

#include <glad/glad.h> // needs to be included before glfw
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fmt/core.h>

#include <graphics/Shader.hpp>
#include <graphics/Camera.hpp>
#include <graphics/Model.hpp>

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

const float SCREEN_WIDTH = 800.0f * 1.5;
const float SCREEN_HEIGTH = 600.0f * 1.5;
const auto WINDOW_TITLE = "demo-model-loading";

float mousePosX = 0.0;
float mousePosY = 0.0;

Camera::ConfigState cameraConfig{
        glm::vec3(0.0f, 0.0f,  10.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f,  0.0f),
        Camera::BoundedData<float>{45.0f, 1.0f, 90.0f},
        Camera::BoundedData<float>{0.0f, -89.f, 89.f},
        Camera::BoundedData<float>{-90.0f, std::nullopt, std::nullopt}, // rotate at x-axis so it looks forward
        Camera::Sensitivity{10.0f, 0.1f},
        glm::vec2{SCREEN_WIDTH/2.0f, SCREEN_HEIGTH/2.0f},
        true
};

Camera camera{cameraConfig};

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processKeyboardInputs(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

int main(int argc, char** argv) {
    fmt::println("main ()");

    glfwInit();
    // same as generated glad library
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGTH, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGTH);

    // event callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    Shader modelWithLightingShader{"resources/shader/model_loading.vert", "resources/shader/model_loading_with_lighting.frag"};
    Model vampireModel{"resources/models/vampire/dancing_vampire.dae"};
    Model backPackModel{"resources/models/backpack/backpack.obj"};

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        processKeyboardInputs(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Current cycle camera view and projection
        const glm::mat4 view = camera.getViewMatrix();
        const glm::mat4 projection = glm::perspective(glm::radians(camera.getFieldOfView()), SCREEN_WIDTH/SCREEN_HEIGTH, 0.1f, 100.0f);

        modelWithLightingShader.use();
        // set shader lighting uniforms
        modelWithLightingShader.setFloat("material.shininess", 32.0f);
        modelWithLightingShader.setInt("numPointLights", 0);
        modelWithLightingShader.setBool("hasSpotLight", false);
        modelWithLightingShader.setVec3Float("directionalLight.direction", -0.2f, -1.0f, -0.3f);
        modelWithLightingShader.setVec3Float("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
        modelWithLightingShader.setVec3Float("directionalLight.diffuse", 0.5f, 0.5f, 0.5f);
        modelWithLightingShader.setVec3Float("directionalLight.specular", 1.0f, 1.0f, 1.0f);
        modelWithLightingShader.setVec3Float("viewPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
        // set shared view, projection value
        modelWithLightingShader.setMat4("projection", projection);
        modelWithLightingShader.setMat4("view", view);
        // draw vampire
        const auto vampireModelMatrix = []{
            const auto identityMatrix = glm::mat4{1.0f};
            const auto scaleMatrix = glm::scale(identityMatrix, glm::vec3{0.05f, 0.05f, 0.05f});
            const auto translateMatrix = glm::translate(identityMatrix, glm::vec3{-5.0f, -5.0f, -13.0f});
            return translateMatrix * scaleMatrix;
        }();
        modelWithLightingShader.setMat4("model", vampireModelMatrix);
        vampireModel.draw(modelWithLightingShader);
        // draw backpack
        const auto backModelMatrix = []{
            const auto identityMatrix = glm::mat4{1.0f};
            const auto rotationMatrix = glm::rotate(identityMatrix, glm::radians(180.0f), glm::vec3{.0f, 1.0f, .0f});
            const auto translateMatrix = glm::translate(identityMatrix, glm::vec3{3.0f, .0f, -3.0f});
            return translateMatrix;
        }();
        modelWithLightingShader.setMat4("model", backModelMatrix);
        backPackModel.draw(modelWithLightingShader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processKeyboardInputs(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processMovement(Camera::Movement::Forward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processMovement(Camera::Movement::Backward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processMovement(Camera::Movement::Left, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processMovement(Camera::Movement::Right, deltaTime);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    camera.processMouseMovement(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(yoffset);
}