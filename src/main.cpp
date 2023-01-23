#include <iostream>
#include <optional>
#include <array>

#include <glad/glad.h> // needs to be included before glfw
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Utils.hpp"
#include "Model.hpp"

#include "TestingData.hpp"
#include "glm/gtc/type_ptr.hpp"

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

const float SCREEN_WIDTH = 800.0f * 1.5;
const float SCREEN_HEIGTH = 600.0f * 1.5;

bool showAxisArrows = true;
bool disableCursor = false;
bool lastDisableCursor = disableCursor;
bool drawWireFrame = false;
bool lastDrawWireFrame = drawWireFrame;
bool enableCameraMouseCallbackMovement = false;
bool enableCulling = false;
bool cullFrontFace = true;
float mousePosX = 0.0;
float mousePosY = 0.0;

Camera::ConfigState cameraConfig{
        glm::vec3(0.0f, 0.0f,  8.0f),
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
bool recalcClipSpace = true;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processKeyboardInputs(GLFWwindow* window);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

int main(int argc, char** argv) {
    std::cout << "main()\n";

    glfwInit();
    // same as generated glad library
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGTH, "LearnOpenGL", nullptr, nullptr);
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

    glfwSetInputMode(window, GLFW_CURSOR, disableCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    glPolygonMode(GL_FRONT_AND_BACK, drawWireFrame ? GL_LINE : GL_FILL);

    // event callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyboardCallback);

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
    // Shader lightShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light.frag"};
    Shader lightShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light_phong.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light_phong.frag"};
    Shader lightSrcShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/light_src.frag"};
    Shader modelShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/model_loading.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/model_loading_with_lighting.frag"};
    // Model model{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/models/backpack/backpack.obj"};
    Shader depthTestingShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/depth_testing.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/depth_testing.frag"};
    Shader stencilTestingShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/stencil_testing.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/stencil_testing.frag"};
    Shader stencilTestingSingleColorShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/stencil_testing.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/stencil_testing.frag"};

    unsigned cubeWithTexCoordsVAO;
    glGenVertexArrays(1, &cubeWithTexCoordsVAO);
    glBindVertexArray(cubeWithTexCoordsVAO);
    unsigned cubeWithTexCoordsVBO;
    glGenBuffers(1, &cubeWithTexCoordsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeWithTexCoordsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testing_data::cubeVerticesWithTextureCoords), testing_data::cubeVerticesWithTextureCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    Shader cubeWithTexCoordsShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/basic_cube.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/basic_cube.frag"};

    unsigned cubeWithNormalsVAO;
    glGenVertexArrays(1, &cubeWithNormalsVAO);
    glBindVertexArray(cubeWithNormalsVAO);
    unsigned cubeWithNormalsVBO;
    glGenBuffers(1, &cubeWithNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeWithNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testing_data::cubeVerticesWithNormals), testing_data::cubeVerticesWithNormals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    unsigned cubeWithNormAndTexCoordVAO;
    glGenVertexArrays(1, &cubeWithNormAndTexCoordVAO);
    glBindVertexArray(cubeWithNormAndTexCoordVAO);
    unsigned cubeWithNormAndTexCoordVBO;
    glGenBuffers(1, &cubeWithNormAndTexCoordVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeWithNormAndTexCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testing_data::cubeVerticesWithNormalsAndTextureCoords), testing_data::cubeVerticesWithNormalsAndTextureCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    Shader skyboxWithEnvMappingShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/skybox_env_mapping.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/skybox_env_mapping.frag"};

    // Indexed drawing
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(testing_data::cubeVerticesWithNormalsAndTextureCoords), testing_data::cubeVerticesWithNormalsAndTextureCoords, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(testing_data::indices), testing_data::indices, GL_STATIC_DRAW);
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

    /*** 3D axis arrows ***/
    constexpr float axisArrowVertices[] = {
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    unsigned int axisArrowsVAO;
    glGenVertexArrays(1, & axisArrowsVAO);
    unsigned int axisArrowsVBO;
    glGenBuffers(1, &axisArrowsVBO);
    glBindVertexArray(axisArrowsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisArrowsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisArrowVertices), axisArrowVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Shader axisArrowShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/axis_arrow.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/axis_arrow.frag"};
    glEnable(GL_LINE_SMOOTH); // line anti-aliasing

    /*** Depth Testing ***/
    // cubes
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    unsigned int cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testing_data::cubeVerticesWithTextureCoords), &testing_data::cubeVerticesWithTextureCoords, GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(testing_data::planeVertices), &testing_data::planeVertices, GL_STATIC_DRAW);
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

    /*** Blending ***/
    constexpr float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    std::vector<glm::vec3> vegetation
    {
            glm::vec3(-1.5f, 0.0f, -0.48f),
            glm::vec3( 1.5f, 0.0f, 0.51f),
            glm::vec3( 0.0f, 0.0f, 0.7f),
            glm::vec3(-0.3f, 0.0f, -2.3f),
            glm::vec3 (0.5f, 0.0f, -0.6f)
    };

    unsigned grassVAO;
    glGenVertexArrays(1, &grassVAO);
    glBindVertexArray(grassVAO);
    unsigned grassVBO;
    glGenBuffers(1, &grassVBO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    const auto textureLoadConfig = []{
        TextureLoadConfig loadConfig;
        loadConfig.flipVertically = false;
        loadConfig.wrapS = GL_CLAMP_TO_EDGE;
        loadConfig.wrapT = GL_CLAMP_TO_EDGE;
        return loadConfig;
    }();
    const auto grassTexture = tryLoadTexture("/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/grass.png", textureLoadConfig);
    if (!grassTexture.has_value()) {
        std::cout << "Failed to load texture: /home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/grass.png\n";
        return 1;
    }
    const auto transparentWindowTexture = tryLoadTexture("/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/blending_transparent_window.png", textureLoadConfig);
    if (!transparentWindowTexture.has_value()) {
        std::cout << "Failed to load texture: /home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/blending_transparent_window.png\n";
        return 1;
    }

    Shader blendingShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/blending.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/blending.frag"};
    blendingShader.use();
    blendingShader.setInt("texture0", 1);

    /*** Face Culling ***/
    unsigned cubeCCWFaceCullingVAO;
    glGenVertexArrays(1, &cubeCCWFaceCullingVAO);
    glBindVertexArray(cubeCCWFaceCullingVAO);
    unsigned cubeCCWFaceCullingVBO;
    glGenBuffers(1, &cubeCCWFaceCullingVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testing_data::cubeVerticesCCW), testing_data::cubeVerticesCCW, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    // intentionally cull front-face
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    /*** Framebuffer ***/
    // TODO: Handle resizing of window
    // texture attachment for color buffer
    unsigned fboColorTextureAttachment;
    glGenTextures(1, &fboColorTextureAttachment);
    glBindTexture(GL_TEXTURE_2D, fboColorTextureAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGTH, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // renderbuffer attachment for depth & stencil buffer
    unsigned fboDepthStencilRboAttachment;
    glGenRenderbuffers(1, &fboDepthStencilRboAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepthStencilRboAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGTH);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // framebuffer setup & adding attachments
    unsigned fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColorTextureAttachment, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboDepthStencilRboAttachment);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR - Framebuffer is not complete\n";
        return 1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // shader
    Shader fboShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/framebuffer_simple.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/framebuffer_simple.frag"};
    fboShader.use();
    fboShader.setInt("texture0", 0);
    // vertices - CCW
    constexpr float fboVertices[] = {
            // pos: x, y, texture: u, v
            -1, -1 , 0, 0,
            -1, 1 , 0, 1,
            1, 1 , 1, 1,

            1, 1 , 1, 1,
            1, -1 , 1, 0,
            -1, -1 , 0, 0,
    };
    // data buffers & shader attributes
    unsigned fboVAO;
    glGenVertexArrays(1, &fboVAO);
    glBindVertexArray(fboVAO);
    unsigned fboVBO;
    glGenBuffers(1, & fboVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fboVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fboVertices), fboVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    // rearview mirror
    // vertices - CCW
    constexpr float fboVerticesRearView[] = {
            // pos: x, y, texture: u, v
            -0.3, 0.5, 0, 0,
            -0.3, 1 , 0, 1,
            0.3, 1 , 1, 1,

            0.3, 1 , 1, 1,
            0.3, 0.5 , 1, 0,
            -0.3, 0.5, 0, 0,
    };
    unsigned fboRearViewVAO;
    glGenVertexArrays(1, &fboRearViewVAO);
    glBindVertexArray(fboRearViewVAO);
    unsigned fboRearViewVBO;
    glGenBuffers(1, & fboRearViewVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fboRearViewVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fboVerticesRearView), fboVerticesRearView, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    /*** Cubemaps ***/
    const float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    unsigned skyboxVAO;
    glGenVertexArrays(1, & skyboxVAO);
    glBindVertexArray(skyboxVAO);
    unsigned skyboxVBO;
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    const std::vector<std::string> cubemapFaces{
        "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/skybox/right.jpg",
        "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/skybox/left.jpg",
        "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/skybox/top.jpg",
        "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/skybox/bottom.jpg",
        "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/skybox/front.jpg",
        "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/texture/skybox/back.jpg",
    };
    const auto skyboxCubemapTexture = loadCubemapTexture(cubemapFaces);
    Shader skyboxShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/skybox.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/skybox.frag"};
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    /*** Advanced Data & GLSL ***/
    // points
    unsigned pointsVAO;
    glGenVertexArrays(1, &pointsVAO);
    unsigned pointsVBO;
    glGenBuffers(1, &pointsVBO);
    glBindVertexArray(pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testing_data::points), testing_data::points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
    glBindVertexArray(0);
    glEnable(GL_PROGRAM_POINT_SIZE);
    Shader pointsShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/advanced_data_glsl/basic.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/advanced_data_glsl/basic.frag"};
    // cube with 4 quadrant
    Shader cube4QuadrantShader{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/advanced_data_glsl/cube.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/advanced_data_glsl/cube_quadrants.frag"};
    cube4QuadrantShader.use();
    cube4QuadrantShader.setInt("texture0", 0);
    // cube with different texture for back face
    Shader cubeFrontBackFaceTextured{"/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/advanced_data_glsl/cube_front_back_face_textured.vert", "/home/kelvin.robles/work/repos/personal/opengl-playground/resources/shader/advanced_data_glsl/cube_front_back_face_textured.frag"};
    cubeFrontBackFaceTextured.use();
    cubeFrontBackFaceTextured.setInt("texture0", 0);
    cubeFrontBackFaceTextured.setInt("texture1", 1);
    // UBO for view, projection
    const unsigned viewProjUBOIndexCubeQuadrant = glGetUniformBlockIndex(cube4QuadrantShader.getId(), "ViewProjection");
    const unsigned viewProjUBOIndexCubeFrontBackFaceTexture = glGetUniformBlockIndex(cubeFrontBackFaceTextured.getId(), "ViewProjection");
    glUniformBlockBinding(cube4QuadrantShader.getId(), viewProjUBOIndexCubeQuadrant, 0);
    glUniformBlockBinding(cubeFrontBackFaceTextured.getId(), viewProjUBOIndexCubeFrontBackFaceTexture, 0);
    unsigned viewProjectionUBO;
    glGenBuffers(1, &viewProjectionUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, viewProjectionUBO);
    // constexpr auto uboViewProjSize = 16 * 4 * 2; // 2 mat4. Per column in mat4 is 16 bytes
    constexpr auto uboViewProjSize = 2 * sizeof(glm::mat4); // same size as above
    glBufferData(GL_UNIFORM_BUFFER, uboViewProjSize, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, viewProjectionUBO);
    // glBindBufferRange(GL_UNIFORM_BUFFER, 0, viewProjectionUBO, 0, uboViewProjSize);

    // z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /*** ImGui ***/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureMouse = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = false;
    int lineWidth = 1;

    float x = 0.0, y = 0.0, z = 0.0;
    glm::vec4 clipSpace{0.0};

    while (!glfwWindowShouldClose(window)) {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        ImGui::Begin("OpenGL Playground");
        ImGui::Text("Hotkeys:");
        ImGui::Text("'H' - show origin axis");
        ImGui::Text("'J' - show/hide cursor");
        ImGui::Text("'K' - wireframe/fill draw");
        ImGui::Text("'X' - pitch +5");
        ImGui::Text("'Z' - pitch -5");
        ImGui::Text("'Y' - yaw +5");
        ImGui::Text("'T' - yaw -5");
        ImGui::NewLine();
        ImGui::Text("Viewport %.0f x %.0f", SCREEN_WIDTH, SCREEN_HEIGTH);
        ImGui::Text("Camera pos:(%.2f, %.2f, %.2f), fov:%.0f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z, camera.getFieldOfView());
        ImGui::Text("Mouse (%.3f, %.3f)", mousePosX, mousePosY);
        ImGui::Checkbox("Enable camera mouse movement", &enableCameraMouseCallbackMovement);
        if (ImGui::Checkbox("Enable Culling", &enableCulling)) {
            if (enableCulling) {
                glEnable(GL_CULL_FACE);
                glCullFace(cullFrontFace ? GL_FRONT : GL_BACK);
            } else {
                glDisable(GL_CULL_FACE);
            }
        }

        if (enableCulling && ImGui::Checkbox("Front face cull", &cullFrontFace)) {
            glCullFace(cullFrontFace ? GL_FRONT : GL_BACK);
        }

        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::NewLine();
        ImGui::Checkbox("Show ImGui demo window", &show_demo_window);
        // debugging clip space and depth buffer computations
        if (ImGui::SliderInt("glLineWidth", &lineWidth, 1, 10)) {
            glLineWidth(lineWidth);
        }
        if (ImGui::InputFloat("x", &x, 1.0f, 1.0f, "%.2f")) {
            recalcClipSpace = true;
        }
        if (ImGui::InputFloat("y", &y, 1.0f, 1.0f, "%.2f")) {
            recalcClipSpace = true;
        }
        if (ImGui::InputFloat("z", &z, 1.0f, 1.0f, "%.2f")) {
            recalcClipSpace = true;
        }
        if (recalcClipSpace) {
            const glm::vec4 worldMat{x, y, z, 1.0};
            const glm::mat4 viewMat = camera.getViewMatrix();
            const glm::mat4 projMat = glm::perspective(glm::radians(camera.getFieldOfView()), SCREEN_WIDTH/SCREEN_HEIGTH, 0.1f, 100.0f);
            clipSpace = projMat * viewMat * worldMat;
            recalcClipSpace= false;
        }
        ImGui::Text("Clip space (%.2f, %.2f, %.2f, %.2f)", clipSpace.x, clipSpace.y, clipSpace.z, clipSpace.w);
        ImGui::Text("Perspective Division (%.2f, %.2f, %.2f, %.2f)", clipSpace.x / clipSpace.w, clipSpace.y/ clipSpace.w , clipSpace.z / clipSpace.w, clipSpace.w / clipSpace.w);
        ImGui::End();
        ImGui::Render();

        processKeyboardInputs(window);

        if (lastDisableCursor != disableCursor) {
            glfwSetInputMode(window, GLFW_CURSOR, disableCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            lastDisableCursor = disableCursor;
        }

        if (lastDrawWireFrame != drawWireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, drawWireFrame ? GL_LINE : GL_FILL);
            lastDrawWireFrame = drawWireFrame;
        }

        // Clear color and depth buffer
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // depth testing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // lambda helper for drawing cube
        auto drawCubeWithTexCoords = [&](const glm::mat4 model, const glm::mat4 view, const glm::mat4 projection, unsigned texture){
            cubeWithTexCoordsShader.use();
            cubeWithTexCoordsShader.setMat4("model", model);
            cubeWithTexCoordsShader.setMat4("view", view);
            cubeWithTexCoordsShader.setMat4("projection", projection);
            glBindVertexArray(cubeWithTexCoordsVAO);
            cubeWithTexCoordsShader.setInt("texture0", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            cubeWithTexCoordsShader.setInt("texture1", 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        };

        // Current cycle camera view and projection
        const glm::mat4 view = camera.getViewMatrix();
        const glm::mat4 projection = glm::perspective(glm::radians(camera.getFieldOfView()), SCREEN_WIDTH/SCREEN_HEIGTH, 0.1f, 100.0f);

        /*** Advanced Data & GLSL ***/
        // set view projection UBO
        glBindBuffer(GL_UNIFORM_BUFFER, viewProjectionUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, uboViewProjSize / 2, glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, uboViewProjSize / 2, uboViewProjSize / 2, glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // points
        glBindVertexArray(pointsVAO);
        pointsShader.use();
        pointsShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(x, y, z)));
        pointsShader.setMat4("view", view);
        pointsShader.setMat4("projection", projection);
        glDrawArrays(GL_POINTS, 0, 3);
        glBindVertexArray(0);
        // cube 4 quadrants
        glBindVertexArray(cubeWithTexCoordsVAO);
        cube4QuadrantShader.use();
        cube4QuadrantShader.setMat4("model", glm::mat4{1.0f});
        // cube4QuadrantShader.setMat4("view", view);
        // cube4QuadrantShader.setMat4("projection", projection);
        cube4QuadrantShader.setFloat("screenWidth", SCREEN_WIDTH);
        cube4QuadrantShader.setFloat("screenHeight", SCREEN_HEIGTH);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureWoodContainer->id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        // cube with different texture for back face
        cubeFrontBackFaceTextured.use();
        glBindVertexArray(cubeCCWFaceCullingVAO);
        cubeFrontBackFaceTextured.use();
        cubeFrontBackFaceTextured.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3{1.25, 1.25, 0.0}));
        // cubeFrontBackFaceTextured.setMat4("view", view);
        // cubeFrontBackFaceTextured.setMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureWoodContainer->id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureAwesomeFace->id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        /*** Cubemaps ***/
//        glEnable(GL_DEPTH_TEST);
//        drawCubeWithTexCoords(glm::mat4{1.0f}, view, projection, textureWoodContainer->id);
//        drawCubeWithTexCoords(glm::translate(glm::mat4{1.0f}, glm::vec3{1.0f, 1.0, 1.0}), view, projection, textureAwesomeFace->id);
//        // cube with reflection
//        skyboxWithEnvMappingShader.use();
//        skyboxWithEnvMappingShader.setInt("skybox", 0);
//        skyboxWithEnvMappingShader.setInt("texture0", 1);
//        skyboxWithEnvMappingShader.setMat4("view", view);
//        skyboxWithEnvMappingShader.setMat4("projection", projection);
//        glBindVertexArray(cubeWithNormAndTexCoordVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapTexture);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, textureWoodContainer->id);
//        // draw cube with 2D texture
//        skyboxWithEnvMappingShader.setMat4("model", glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3{-6.0f, 0.0f, -2.0}), glm::vec3{2.0, 2.0, 2.0}));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        // draw cube with skybox reflection
//        skyboxWithEnvMappingShader.setBool("applyReflection", true);
//        skyboxWithEnvMappingShader.setMat4("model", glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3{-3.0f, 0.0f, 0.0}), glm::vec3{2.0, 2.0, 2.0}));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        skyboxWithEnvMappingShader.setBool("applyReflection", false);
//        // draw cube with skybox refraction
//        skyboxWithEnvMappingShader.setBool("applyRefraction", true);
//        skyboxWithEnvMappingShader.setMat4("model", glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3{-9.0f, 0.0f, -2.0}), glm::vec3{2.0, 2.0, 2.0}));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        skyboxWithEnvMappingShader.setBool("applyRefraction", false);
//        // draw skybox
//        // vertex shader modified to always output 1 for z-buffer / depth, by doing so
//        // GL_LEQUAL will only pass if there is no depth value stored in depth buffer
//        // which means no fragment was drawn at the given screen space coordinate
//        glDepthFunc(GL_LEQUAL);
//        skyboxShader.use();
//        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view))); // remove the translation part of view matrix
//        skyboxShader.setMat4("projection", projection);
//        glBindVertexArray(skyboxVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemapTexture);
//        glDrawArrays(GL_TRIANGLES, 0, sizeof(skyboxVertices)/sizeof(decltype(skyboxVertices[0])));
//        glDepthFunc(GL_LESS);

//        /*** Framebuffer - 1 ***/
//        // should uncomment `Depth Testing` section for drawing in framebuffer
//        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//        glEnable(GL_DEPTH_TEST);
//
//        /*** Depth Testing ***/
//        auto runDepthTestingCode = [&](const glm::mat4& viewMatrix) {
//            depthTestingShader.use();
////        depthTestingShader.setBool("visualizeDepthLinear", true);
//            depthTestingShader.setMat4("view", viewMatrix);
//            depthTestingShader.setMat4("projection", projection);
//            // cubes
//            glBindVertexArray(cubeVAO);
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D, woodContainerDiffuseMap->id);
//            depthTestingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(-1.0f, 0.0f, -1.0f)));
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//            depthTestingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(2.0f, 0.0f, 0.0f)));
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//            // floor
//            glBindVertexArray(planeVAO);
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D, floorTexture->id);
//            depthTestingShader.setMat4("model", glm::mat4{1.0f});
//            glDrawArrays(GL_TRIANGLES, 0, 6);
//        };
//        runDepthTestingCode(camera.getViewMatrix());
//
//        /*** Framebuffer - 2 ***/
//        auto drawOnMainFrameBufferUsingFBOColorAttachment = [&](unsigned vao, bool clearColorBuffer = true, bool disableDepthTestBeforeDrawing = false){
//            glBindFramebuffer(GL_FRAMEBUFFER, 0);
//            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//            if (clearColorBuffer) {
//                glClear(GL_COLOR_BUFFER_BIT);
//            }
//            fboShader.use();
////        fboShader.setBool("inversion", true);
////        fboShader.setBool("grayScaleAverage", true);
////        fboShader.setBool("grayScaleWeighted", true);
////        fboShader.setBool("kernelEffectSharpen", true);
////        fboShader.setBool("kernelEffectBlurr", true);
////        fboShader.setBool("blurrStrength", 64);
////        fboShader.setBool("kernelEffectEdgeDetection", true);
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D, fboColorTextureAttachment);
//            glBindVertexArray(vao);
//            if (disableDepthTestBeforeDrawing) {
//                glDisable(GL_DEPTH_TEST);
//            }
//            glDrawArrays(GL_TRIANGLES, 0, 6);
//            glDisable(GL_DEPTH_TEST); // disable depth test so axis will showup
//        };
//        drawOnMainFrameBufferUsingFBOColorAttachment(fboVAO);
//
//        // rearview mirror
//        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//        glEnable(GL_DEPTH_TEST);
//        // TODO: Understand why this rotate gives wrong result when camera is position on top and looking down
//        camera.setClampPitchEnabled(false);
//        camera.rotate(-180, 0);
//        runDepthTestingCode(camera.getViewMatrix());
//        camera.setClampPitchEnabled(true);
//        camera.rotate(180, 0);
//        drawOnMainFrameBufferUsingFBOColorAttachment(fboRearViewVAO, false, true);

        /*** Face Culling ***/
//        // reuse blending shader
//        blendingShader.use();
//        blendingShader.setMat4("view", view);
//        blendingShader.setMat4("projection", projection);
//        // draw cube
//        glBindVertexArray(cubeCCWFaceCullingVAO);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, cubeTexture->id);
//        blendingShader.setMat4("model", glm::mat4{1.0f});
//        glDrawArrays(GL_TRIANGLES, 0, 36);


//        /*** Blending ***/
//        blendingShader.use();
//        blendingShader.setMat4("view", view);
//        blendingShader.setMat4("projection", projection);
//        // draw floor
//        glBindVertexArray(planeVAO);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, floorTexture->id);
//        blendingShader.setMat4("model", glm::mat4{1.0f});
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        // draw cubes
//        glBindVertexArray(cubeVAO);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, cubeTexture->id);
//        blendingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(-1.0f, 0.0f, -1.0f)));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        blendingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(2.0f, 0.0f, 0.0f)));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        // draw grass or window
//        glBindVertexArray(grassVAO);
//        glActiveTexture(GL_TEXTURE1);
////        glBindTexture(GL_TEXTURE_2D, grassTexture->id);
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glBindTexture(GL_TEXTURE_2D, transparentWindowTexture->id);
//        // sort from farthest to nearest position, so depth test won't interfere with blending
//        auto sortedPosition = vegetation;
//        std::sort(sortedPosition.begin(), sortedPosition.end(), [camPos = camera.getPosition()](const auto& left, const auto& right){
//            const auto leftDist = glm::distance(camPos, left);
//            const auto rightDist = glm::distance(camPos, right);
//            return leftDist > rightDist;
//        });
//        for (auto idx = 0u; idx < sortedPosition.size(); idx++) {
//            blendingShader.setMat4("model", glm::translate(glm::mat4(1.0f), sortedPosition[idx]));
//            glDrawArrays(GL_TRIANGLES, 0, 6);
//        }

        /*** Stencil Testing ***/
//        stencilTestingShader.use();
//        stencilTestingShader.setMat4("view", view);
//        stencilTestingShader.setMat4("projection", projection);
//        glEnable(GL_STENCIL_TEST);
//        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // when both depth & stencil test passed, write stencil value equal to `REF` which is 1
//        // draw floor
//        glBindVertexArray(planeVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, floorTexture->id);
//        stencilTestingShader.setMat4("model", glm::mat4{1.0f});
//        glStencilMask(0x00); // AND bit mask will always make the value to be written 0
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        // draw cube with stencil write on
//        glBindVertexArray(cubeVAO);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, cubeTexture->id);
//        glStencilFunc(GL_ALWAYS, 1, 0xFF); // all drawn fragments will pass stencil testing
//        glStencilMask(0xFF); // AND bit mask now will write whatever stencil value is to be written
//        stencilTestingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(-1.0f, 0.0f, -1.0f)));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        stencilTestingShader.setMat4("model", glm::translate(glm::mat4{1.0f}, glm::vec3(2.0f, 0.0f, 0.0f)));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        // draw up-scaled colored cube with border with stencil write off
//        stencilTestingSingleColorShader.use();
//        stencilTestingSingleColorShader.setMat4("view", view);
//        stencilTestingSingleColorShader.setMat4("projection", projection);
//        stencilTestingSingleColorShader.setBool("useUniformColor", true);
//        stencilTestingSingleColorShader.setVec3Float("color", 1.0, 0.0, 0.0);
//        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // stored fragments with stencil buffer not equal to 1 will pass
//        glStencilMask(0x00); // AND bit mask will always make the value to be written 0
//        glDisable(GL_DEPTH_TEST); // disable depth testing to make sure the border will always be drawn over the cubes, even if it's behind/below the floor
//        stencilTestingSingleColorShader.setMat4("model", glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3(-1.0f, 0.0f, -1.0f)), glm::vec3(1.1f, 1.1f, 1.1f)));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        stencilTestingSingleColorShader.setMat4("model", glm::scale(glm::translate(glm::mat4{1.0f}, glm::vec3(2.0f, 0.0f, 0.0f)), glm::vec3(1.1f, 1.1f, 1.1f)));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//        // reset back to default
//        glEnable(GL_DEPTH_TEST);
//        // TODO: Understand how the below really affects glClear(GL_STENCIL_BUFFER_BIT)
//        glStencilFunc(GL_ALWAYS, 1, 0xFF); // this also seems to influence glClear(GL_STENCIL_BUFFER_BIT)
//        glStencilMask(0xFF); // glClear(GL_STENCIL_BUFFER_BIT) seems to not work properly without this

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
        // cubeWithTexCoordsShader.use();
        // cubeWithTexCoordsShader.setMat4("view", view);
        // cubeWithTexCoordsShader.setMat4("projection", projection);
        // // purposely flip the value of this to test GL_TEXTUREN value mapping
        // cubeWithTexCoordsShader.setInt("texture0", 1); // 1 - maps to GL_TEXTURE1
        // cubeWithTexCoordsShader.setInt("texture1", 0); // 0 - maps to GL_TEXTURE0
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, textureWoodContainer); // maps to texture0 uniform
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, textureAwesomeFace); // maps to texture1 uniform
        // for (auto i = 0u; i < 10; i++) {
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, cubePositions[i]);
        //     float angle = 20.0f * i;
        //     model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //     cubeWithTexCoordsShader.setMat4("model", model);
        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        /*** Render by re-using vertices through EBO ***/
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /*** 3D axis arrows ***/
        if (showAxisArrows) {
            glDisable(GL_DEPTH_TEST);
            axisArrowShader.use();
            axisArrowShader.setMat4("model", glm::mat4(1.0f));
            axisArrowShader.setMat4("view", view);
            axisArrowShader.setMat4("projection", projection);
            glBindVertexArray(axisArrowsVAO);
            glLineWidth(4);
            glDrawArrays(GL_LINES, 0, 6);
            glLineWidth(lineWidth);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);
        }

        // Render ImGui last to make sure it will be drawn on top of the scene
        // and won't get affected by any drawing & manipulation that is happening
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
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

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const auto actionStr = [&]{
        switch (action) {
            case GLFW_PRESS:
                return "GLFW_PRESS";
            case GLFW_RELEASE:
                return "GLFW_RELEASE";
            case GLFW_REPEAT:
                return "GLFW_REPEAT";
            default:
                return "Unknown";
        }
    }();

    // handle toggles here, since `processKeyboardInputs` is being called multiple times in a single frame
    if (action == GLFW_PRESS) {
        std::optional<std::string> keyStr;
        switch (key) {
            case GLFW_KEY_H:
                keyStr = "H";
                showAxisArrows = !showAxisArrows;
                break;
            case GLFW_KEY_J:
                keyStr = "J";
                disableCursor = !disableCursor;
                break;
            case GLFW_KEY_K:
                keyStr = "K";
                drawWireFrame = !drawWireFrame;
                break;
            case GLFW_KEY_X:
                keyStr = "X";
                std::cout << "yaw camera.rotate(5, 0) \n";
                camera.rotate(5, 0);
                break;
            case GLFW_KEY_Z:
                keyStr = "Z";
                std::cout << "yaw camera.rotate(-5, 0) \n";
                camera.rotate(-5, 0);
                break;
            case GLFW_KEY_Y:
                keyStr = "Y";
                std::cout << "pitch camera.rotate(0, 5) \n";
                camera.rotate(0, 5);
                break;
            case GLFW_KEY_T:
                keyStr = "T";
                std::cout << "pitch camera.rotate(0, -5) \n";
                camera.rotate(0, -5);
                break;
        }

        if (keyStr.has_value()) {
            std::cout << "keyboardCallback " << actionStr << " " << keyStr.value() << "\n";
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (enableCameraMouseCallbackMovement) {
        camera.processMouseMovement(xpos, ypos);
    }
    mousePosX = xpos;
    // GLFW originates from top-left, OpenGL is bottom-left
    mousePosY = SCREEN_HEIGTH - ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
    recalcClipSpace = true;
}
