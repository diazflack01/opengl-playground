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
const auto WINDOW_TITLE = APP_NAME;
constexpr glm::mat4 IDENTITY_MATRIX{1.0f};

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

    float cubeVertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // set VAO data layout attributes for vertex shader usage
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // unbind VAO
    glBindVertexArray(0);

    Shader lightCubeShader{"resources/shader/demo_phong_lighting_light.vert", "resources/shader/demo_phong_lighting_light.frag"};
    Shader modelWithLightingShader{"resources/shader/model_loading.vert", "resources/shader/model_loading_with_lighting.frag"};
    Model vampireModel{"resources/models/vampire/dancing_vampire.dae"};
    // Model backPackModel{"resources/models/backpack/backpack.obj"};

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

        // draw light cube
        glBindVertexArray(VAO);
        const auto currTime = glfwGetTime();
        // const glm::vec3 lightPos{sin(currTime) * 1.2f, cos(currTime) * 1.0f, sin(currTime) * 2.0f};
        const auto lightCubeScaleMatrix = glm::scale(IDENTITY_MATRIX, glm::vec3{0.2f});
        const std::vector<glm::vec3> lightPositions = {{-5.f + (sin(currTime) * 3.f), 5.f, -10.f}, {-5.f, 2.f + (cos(currTime) * 3.0f), -11.f}, {-5.f + (sin(currTime) * 3.f), -4.f, -10.f}};
        lightCubeShader.use();
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", projection);
        for (const auto& lightPos : lightPositions) {
            const auto lightCubeTranslateMatrix = glm::translate(IDENTITY_MATRIX, lightPos);
            lightCubeShader.setMat4("model", lightCubeTranslateMatrix * lightCubeScaleMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        modelWithLightingShader.use();
        // set shader lighting uniforms
        // material property
        modelWithLightingShader.setFloat("material.shininess", 32.0f);
        // point lights
        modelWithLightingShader.setInt("numPointLights", lightPositions.size());
        for (auto i = 0u; i < lightPositions.size(); i++) {
            const auto pointLightPrefixStr = "pointLights[" + std::to_string(i) + "]";
            modelWithLightingShader.setVec3Float(pointLightPrefixStr + ".position", lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
            modelWithLightingShader.setFloat(pointLightPrefixStr + ".constant", 1.0f);
            modelWithLightingShader.setFloat(pointLightPrefixStr + ".linear", 0.09f);
            modelWithLightingShader.setFloat(pointLightPrefixStr + ".quadratic", .032f);
            modelWithLightingShader.setVec3Float(pointLightPrefixStr + ".ambient", 0.2f, 0.2f, 0.2f);
            modelWithLightingShader.setVec3Float(pointLightPrefixStr + ".diffuse", 0.5f, 0.5f, 0.5f);
            modelWithLightingShader.setVec3Float(pointLightPrefixStr + ".specular", 1.0f, 1.0f, 1.0f);
        }
        // spotlight
        modelWithLightingShader.setBool("hasSpotLight", true);
        modelWithLightingShader.setVec3Float("spotLight.position", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
        modelWithLightingShader.setVec3Float("spotLight.direction", camera.getFront().x, camera.getFront().y, camera.getFront().z);
        modelWithLightingShader.setFloat("spotLight.cutOff", 12.5);
        modelWithLightingShader.setFloat("spotLight.outerCutOff", 17.5);
        modelWithLightingShader.setFloat("spotLight.constant", 1.0f);
        modelWithLightingShader.setFloat("spotLight.linear", 0.09f);
        modelWithLightingShader.setFloat("spotLight.quadratic", .032f);
        modelWithLightingShader.setVec3Float("spotLight.ambient", 0.1f, 0.1f, 0.1f);
        modelWithLightingShader.setVec3Float("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        modelWithLightingShader.setVec3Float("spotLight.specular", 1.0f, 1.0f, 1.0f);
        // directional light
        modelWithLightingShader.setVec3Float("directionalLight.direction", -0.2f, -1.0f, -0.3f);
        modelWithLightingShader.setVec3Float("directionalLight.ambient", 0.01f, 0.01f, 0.01f);
        modelWithLightingShader.setVec3Float("directionalLight.diffuse", 0.1f, 0.1f, 0.1f);
        modelWithLightingShader.setVec3Float("directionalLight.specular", 1.0f, 1.0f, 1.0f);
        modelWithLightingShader.setVec3Float("viewPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
        // set shared view, projection value
        modelWithLightingShader.setMat4("projection", projection);
        modelWithLightingShader.setMat4("view", view);
        // draw vampire
        const auto vampireModelMatrix = []{
            const auto scaleMatrix = glm::scale(IDENTITY_MATRIX, glm::vec3{0.05f, 0.05f, 0.05f});
            const auto translateMatrix = glm::translate(IDENTITY_MATRIX, glm::vec3{-5.0f, -5.0f, -13.0f});
            return translateMatrix * scaleMatrix;
        }();
        modelWithLightingShader.setMat4("model", vampireModelMatrix);
        vampireModel.draw(modelWithLightingShader);
        // draw backpack
        const auto backModelMatrix = []{
            const auto rotationMatrix = glm::rotate(IDENTITY_MATRIX, glm::radians(180.0f), glm::vec3{.0f, 1.0f, .0f});
            const auto translateMatrix = glm::translate(IDENTITY_MATRIX, glm::vec3{3.0f, .0f, -3.0f});
            return translateMatrix;
        }();
        modelWithLightingShader.setMat4("model", backModelMatrix);
        // backPackModel.draw(modelWithLightingShader);

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