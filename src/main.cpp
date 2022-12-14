#include <iostream>
#include <optional>
#include <array>

#include <glad/glad.h> // needs to be included before glfw
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Utils.hpp"
#include "Model.hpp"

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGTH = 600.0f;

Camera::ConfigState cameraConfig{
        glm::vec3(0.0f, 0.0f,  8.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f,  0.0f),
        Camera::BoundedData<float>{45.0f, 1.0f, 45.0f},
        Camera::BoundedData<float>{0.0f, -89.f, 89.f},
        Camera::BoundedData<float>{-90.0f, std::nullopt, std::nullopt}, // rotate at x-axis so it looks forward
        Camera::Sensitivity{2.5f, 0.1f},
        glm::vec2{SCREEN_WIDTH/2.0f, SCREEN_HEIGTH/2.0f}
};

Camera camera{cameraConfig};

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processKeyboardInputs(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

int main(int argc, char** argv) {
    std::cout << "main()\n";

    glfwInit();
    // same as generated glad library
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
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

    glViewport(0, 0, 800, 600);

    // disable mouse cursor
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // event callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    const auto textureWoodContainer = tryLoadTexture("/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/wooden_container.jpg");
    if (!textureWoodContainer.has_value()) {
        std::cout << "Failed to load texture: /home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/wooden_container.jpg\n";
        return 1;
    }

    const auto textureAwesomeFace = tryLoadTexture("/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/awesomeface.png");
    if (!textureAwesomeFace.has_value()) {
        std::cout << "Failed to load texture: /home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/awesomeface.png\n";
        return 1;
    }

    const auto woodContainerDiffuseMap = tryLoadTexture("/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/container2.png");
    if (!woodContainerDiffuseMap.has_value()) {
        std::cout << "Failed to load texture: /home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/container2.png\n";
        return 1;
    }
    const auto woodContainerSpecularMap = tryLoadTexture("/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/container2_specular.png");
    if (!woodContainerSpecularMap.has_value()) {
        std::cout << "Failed to load texture: /home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/container2_specular.png\n";
        return 1;
    }

    // Shader shader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/basic.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/basic.frag"};
    // Shader shader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/basic_texture.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/basic_texture.frag"};
    // Shader texturedShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/basic_cube.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/basic_cube.frag"};
    // Shader lightShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light.frag"};
    Shader lightShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light_phong.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light_phong.frag"};
    Shader lightSrcShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light_src.frag"};
    Shader modelShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/model_loading.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/model_loading_with_lighting.frag"};
    // Model model{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/models/backpack/backpack.obj"};
    Shader depthTestingShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/depth_testing.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/depth_testing.frag"};
    Shader stencilTestingShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/stencil_testing.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/stencil_testing.frag"};
    Shader stencilTestingSingleColorShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/stencil_testing.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/stencil_testing.frag"};

    const float vertices[] = {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left
    };
    const unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
    };

    float verticesWithTextureCoords[] = {
            // positions          // colors           // texture coords
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };

    const float coloredTriangleVertices[] = {
            // positions         // colors
            0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
            -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
            0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
    };

    float cubeVerticesWithTextureCoords[] = {
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

    const float planeVertices[] = {
            // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
            5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

            5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
            5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    const float cubeVerticesWithNormals[] = {
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

    const float cubeVerticesWithNormalsAndTextureCoords[] = {
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

    constexpr glm::vec3 cubePositions[] = {
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

    constexpr std::array<glm::vec3, 4> pointLightPositions{
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // NOTES:
    // - need to bind VAO to store glVertexAttribPointer call below
    // - binded VBO, EBO are also stored
    // - any update in bind (VBO, EBO) will be stored
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesWithNormalsAndTextureCoords), cubeVerticesWithNormalsAndTextureCoords, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // shader attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // unbind VAO
    glEnableVertexAttribArray(0);

    unsigned lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*** Depth Testing ***/
    // cubes
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    unsigned int cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesWithTextureCoords), &cubeVerticesWithTextureCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // floor
    unsigned int planeVAO;
    glGenVertexArrays(1, &planeVAO);
    unsigned int planeVBO;
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // textures
    const auto cubeTexture = tryLoadTexture("/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/marble.jpg");
    if (!cubeTexture.has_value()) {
        std::cout << "Failed to load texture: /home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/marble.jpg\n";
        return 1;
    }
    const auto floorTexture = tryLoadTexture("/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/metal.png");
    if (!floorTexture.has_value()) {
        std::cout << "Failed to load texture: /home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/metal.png\n";
        return 1;
    }

    depthTestingShader.use();
    depthTestingShader.setInt("texture0", 0);

    // wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    while (!glfwWindowShouldClose(window)) {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processKeyboardInputs(window);

        // Clear color and depth buffer
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // depth testing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Current cycle camera view and projection
        const glm::mat4 view = camera.getViewMatrix();
        const glm::mat4 projection = glm::perspective(glm::radians(camera.getFieldOfView()), SCREEN_WIDTH/SCREEN_HEIGTH, 0.1f, 100.0f);

        /*** Depth Testing ***/
//        depthTestingShader.use();
//        depthTestingShader.setMat4("view", view);
//        depthTestingShader.setMat4("projection", projection);
//        // cubes
//        glBindVertexArray(cubeVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, cubeTexture->id);
//        depthTestingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(-1.0f, 0.0f, -1.0f)));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        depthTestingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(2.0f, 0.0f, 0.0f)));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        // floor
//        glBindVertexArray(planeVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, floorTexture->id);
//        depthTestingShader.setMat4("model", glm::mat4{1.0f});
//        glDrawArrays(GL_TRIANGLES, 0, 6);

        /*** Stencil Testing ***/
        stencilTestingShader.use();
        stencilTestingShader.setMat4("view", view);
        stencilTestingShader.setMat4("projection", projection);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // when both depth & stencil test passed, write stencil value equal to `REF` which is 1
        // draw floor
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture->id);
        stencilTestingShader.setMat4("model", glm::mat4{1.0f});
        glStencilMask(0x00); // AND bit mask will always make the value to be written 0
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // draw cube with stencil write on
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture->id);
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // all drawn fragments will pass stencil testing
        glStencilMask(0xFF); // AND bit mask now will write whatever stencil value is to be written
        stencilTestingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(-1.0f, 0.0f, -1.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        stencilTestingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(2.0f, 0.0f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // draw up-scaled colored cube with border with stencil write off
        stencilTestingSingleColorShader.use();
        stencilTestingSingleColorShader.setMat4("view", view);
        stencilTestingSingleColorShader.setMat4("projection", projection);
        stencilTestingSingleColorShader.setBool("useUniformColor", true);
        stencilTestingSingleColorShader.setVec3Float("color", 1.0, 0.0, 0.0);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // stored fragments with stencil buffer not equal to 1 will pass
        glStencilMask(0x00); // AND bit mask will always make the value to be written 0
        glDisable(GL_DEPTH_TEST); // disable depth testing to make sure the border will always be drawn over the cubes, even if it's behind/below the floor
        stencilTestingSingleColorShader.setMat4("model", glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3(-1.0f, 0.0f, -1.0f)), glm::vec3(1.1f, 1.1f, 1.1f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        stencilTestingSingleColorShader.setMat4("model", glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3(2.0f, 0.0f, 0.0f)), glm::vec3(1.1f, 1.1f, 1.1f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // reset back to default
        glEnable(GL_DEPTH_TEST);
        // TODO: Understand how the below really affects glClear(GL_STENCIL_BUFFER_BIT)
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // this also seems to influence glClear(GL_STENCIL_BUFFER_BIT)
        glStencilMask(0xFF); // glClear(GL_STENCIL_BUFFER_BIT) seems to not work properly without this

        /*** Model Loading ***/
//        modelShader.use();
//        modelShader.setMat4("model", glm::mat4{1.0f});
//        modelShader.setMat4("view", view);
//        modelShader.setMat4("projection", projection);
//        const auto camPos = camera.getPosition();
//        modelShader.setVec3Float("viewPos", camPos.x, camPos.y, camPos.z);
//        // directional light
//        constexpr glm::vec3 ambient{0.5f, 0.5f, 0.5f};
//        constexpr glm::vec3 specular{0.8f, 0.8f, 0.8f};
//        modelShader.setVec3Float("directionalLight.ambient",  ambient.x, ambient.y, ambient.z);
//        modelShader.setVec3Float("directionalLight.diffuse",  0.4f, 0.4f, 0.4f);
//        modelShader.setVec3Float("directionalLight.specular", specular.x, specular.y, specular.z);
//        constexpr glm::vec3 lightDirection{-0.2f, -1.0f, -0.3f};
//        modelShader.setVec3Float("directionalLight.direction", lightDirection.x, lightDirection.y, lightDirection.z);
//        modelShader.setBool("hasSpotLight", false);
//        modelShader.setInt("numPointLights", 0);
//        model.draw(modelShader);

        /*** Light Source ***/
//        lightSrcShader.use();
//        lightSrcShader.setMat4("view", view);
//        lightSrcShader.setMat4("projection", projection);
//
//        for (auto i = 0u; i < pointLightPositions.size(); i++) {
//            const glm::mat4 lightModel = [&]{
//                auto model = glm::translate(glm::mat4{1.0f}, pointLightPositions[i]);
//                return glm::scale(model, glm::vec3{0.2f});
//            }();
//            lightSrcShader.setMat4("model", lightModel);
//            glBindVertexArray(lightVAO);
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//        }

        /*** Object/s with lighting shader applied  ***/
//        lightShader.use();
//        lightShader.setMat4("view", view);
//        lightShader.setMat4("projection", projection);
//        const auto camPos = camera.getPosition();
//        lightShader.setVec3Float("viewPos", camPos.x, camPos.y, camPos.z);
//        // object material
//        lightShader.setInt("material.diffuse", 0);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, woodContainerDiffuseMap->id);
//        lightShader.setInt("material.specular", 1);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, woodContainerSpecularMap->id);
//        lightShader.setFloat("material.shininess", 32.0f);
//        // directional light
//        constexpr glm::vec3 ambient{0.5f, 0.5f, 0.5f};
//        constexpr glm::vec3 specular{1.0f, 1.0f, 1.0f};
//        lightShader.setVec3Float("directionalLight.ambient",  ambient.x, ambient.y, ambient.z);
//        lightShader.setVec3Float("directionalLight.diffuse",  0.4f, 0.4f, 0.4f);
//        lightShader.setVec3Float("directionalLight.specular", specular.x, specular.y, specular.z);
//        constexpr glm::vec3 lightDirection{-0.2f, -1.0f, -0.3f};
//        lightShader.setVec3Float("directionalLight.direction", lightDirection.x, lightDirection.y, lightDirection.z);
//        // point lights
//        for (auto i = 0u; i < pointLightPositions.size(); i++) {
//            const auto pointLightIdx = [&]{
//                std::string pointLight = "pointLights[";
//                pointLight.append(std::to_string(i));
//                pointLight.append("]");
//                return pointLight;
//            }();
//
//            lightShader.setVec3Float(pointLightIdx + ".ambient",  ambient.x, ambient.y, ambient.z);
//            lightShader.setVec3Float(pointLightIdx + ".diffuse",  0.8, 0.8, 0.8);
//            lightShader.setVec3Float(pointLightIdx + ".specular", specular.x, specular.y, specular.z);
//            lightShader.setFloat(pointLightIdx + ".constant",  1.0f);
//            lightShader.setFloat(pointLightIdx + ".linear",    0.09f);
//            lightShader.setFloat(pointLightIdx + ".quadratic", 0.032f);
//        }
//        // spotlight
//        lightShader.setVec3Float("spotLight.ambient",  0.1f, 0.1f, 0.1f);
//        lightShader.setVec3Float("spotLight.diffuse",  1.0f, 1.0f, 1.0f);
//        lightShader.setVec3Float("spotLight.specular", specular.x, specular.y, specular.z);
//        const auto camFront = camera.getFront();
//        lightShader.setVec3Float("spotLight.direction", camFront.x, camFront.y, camFront.z);
//        lightShader.setVec3Float("spotLight.position", camPos.x, camPos.y, camPos.z);
//        lightShader.setFloat("spotLight.constant", 1.0f);
//        lightShader.setFloat("spotLight.linear", 0.09f);
//        lightShader.setFloat("spotLight.quadratic", 0.032f);
//        lightShader.setFloat("spotLight.cutOff",   glm::cos(glm::radians(12.5f)));
//        lightShader.setFloat("spotLight.outerCutOff",   glm::cos(glm::radians(15.0f)));
//
//        // multiple cube objects
//        glBindVertexArray(VAO);
//        for(unsigned int i = 0; i < 10; i++)
//        {
//            glm::mat4 model = glm::mat4(1.0f);
//            model = glm::translate(model, cubePositions[i]);
//            float angle = 20.0f * i;
//            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//            lightShader.setMat4("model", model);
//
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//        }

        /*** Multiple Cubes ***/
        // texturedShader.use();
        // texturedShader.setMat4("view", view);
        // texturedShader.setMat4("projection", projection);
        // // purposely flip the value of this to test GL_TEXTUREN value mapping
        // texturedShader.setInt("texture0", 1); // 1 - maps to GL_TEXTURE1
        // texturedShader.setInt("texture1", 0); // 0 - maps to GL_TEXTURE0
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, textureWoodContainer); // maps to texture0 uniform
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, textureAwesomeFace); // maps to texture1 uniform
        // for (auto i = 0u; i < 10; i++) {
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, cubePositions[i]);
        //     float angle = 20.0f * i;
        //     model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //     texturedShader.setMat4("model", model);
        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        /*** Render by re-using vertices through EBO ***/
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // depth testing
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}
