#include <vector>

#include <fmt/core.h>

#include <graphics/WindowManager.hpp>
#include <GLFW/glfw3.h>
#include <graphics/Shader.hpp>
#include <graphics/Mouse.hpp>
#include <graphics/Camera.hpp>
#include <graphics/VertexBuffer.hpp>
#include <graphics/VertexArray.hpp>
#include <graphics/IndexBuffer.hpp>
#include <graphics/Texture.hpp>

#include <utils/Utils.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGTH = 600.0f;
const auto WINDOW_TITLE = APP_NAME;
constexpr glm::mat4 IDENTITY_MATRIX{1.0f};

const glm::vec3 cameraPos   = glm::vec3(4.5f, .7f,  0.0f);
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

void imguiHelperFunc(ImGuiIO& io, WindowManager& window);

int main(int argc, char** argv) {
    fmt::println("main ()");

    WindowManager windowManager{SCREEN_WIDTH, SCREEN_HEIGTH, WINDOW_TITLE};

    // mouse move and scroll callbacks
    glfwSetCursorPosCallback(windowManager.getWindow(), mouseMoveCallback);
    glfwSetScrollCallback(windowManager.getWindow(), mouseScrollCallback);
    glfwSetMouseButtonCallback(windowManager.getWindow(), mouseButtonCallback);

    const std::vector<float> cubeVertices = {
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
    VertexBuffer cubeVBO{cubeVertices};
    VertexAttributesLayout cubeVAOLayout{};
    cubeVAOLayout.add(3, GL_FLOAT, false);
    cubeVAOLayout.add(3, GL_FLOAT, false);
    cubeVAOLayout.add(2, GL_FLOAT, false);
    VertexArray cubeVAO{cubeVBO, cubeVAOLayout};

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    const std::vector<float> planeVertices = {
            // positions            // normals         // texcoords
            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

            10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
            10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    VertexBuffer planeVBO{planeVertices};
    VertexAttributesLayout planeVAOLayout{};
    planeVAOLayout.add(3, GL_FLOAT, false);
    planeVAOLayout.add(3, GL_FLOAT, false);
    planeVAOLayout.add(2, GL_FLOAT, false);
    VertexArray planeVAO{planeVBO, planeVAOLayout};

    Shader shader{"resources/shader/demo_phong_lighting.vert", "resources/shader/demo_phong_lighting.frag"};
    Texture cubeTexture{"resources/texture/marble.jpg", 0};
    Texture floorTexture{"resources/texture/metal.png", 0};

    // draw wireframe. Default is GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Setup Dear ImGui Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.WantCaptureMouse = true;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(windowManager.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // rotate the camera -90 deg to the left and -10 deg to look down
    camera.rotate(-90, -10);

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

        // mvp
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        const auto& camPos = camera.getPosition();
        shader.setVec3Float("viewPos", camPos.x, camPos.y, camPos.z);
        // light
        shader.setVec3Float("light.position",  1.0f, 1.0f, 1.0f);
        shader.setVec3Float("light.ambient",  1.0f, 1.0f, 1.0f);
        shader.setVec3Float("light.diffuse",  1.0f, 1.0f, 1.0f);
        shader.setBool("useSpecular", false);

        // cubes
        cubeVAO.bind();
        cubeTexture.bind(12);
        shader.setInt("material.diffuse",  12);
        auto model = IDENTITY_MATRIX;
        // cube 1
        model = glm::translate(IDENTITY_MATRIX, glm::vec3{-1.0f, 0.0f, -1.0f});
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // cube 2
        model = glm::translate(IDENTITY_MATRIX, glm::vec3{2.0f, 0.0f, 0.0f});
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // floor
        planeVAO.bind();
        floorTexture.bind(12);
        shader.setMat4("model", IDENTITY_MATRIX);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        imguiHelperFunc(io, windowManager);


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

void imguiHelperFunc(ImGuiIO& io, WindowManager& window) {
    static bool showImGuiDemoWindow = false;
    static bool enableVSync = true;
    static bool hideMouseCursor = false;
    static bool drawWireFrame = false;
    static bool enableDepthTesting = true;
    static auto depthFunc = GL_ALWAYS;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showImGuiDemoWindow)
        ImGui::ShowDemoWindow(&showImGuiDemoWindow);

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Settings");
        ImGui::Checkbox("ImGui Demo Window", &showImGuiDemoWindow);
        ImGui::Checkbox("V-Sync", &enableVSync);
        ImGui::Checkbox("Hide Mouse Cursor", &hideMouseCursor);
        ImGui::Checkbox("Wireframe drawing", &drawWireFrame);
        ImGui::Checkbox("Depth testing", &enableDepthTesting);
        if (enableDepthTesting) {
            const char* depthTestFunc[] = {"GL_ALWAYS", "GL_LESS", "GL_NEVER", "GL_EQUAL", "GL_LEQUAL", "GL_GREATER", "GL_NOTEQUAL", "GL_GEQUAL"};
            constexpr std::array<int, 9> DEPTH_FUNC_VAL = {GL_ALWAYS, GL_LESS, GL_NEVER, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL};
            static int depthTestFuncCurrentIdx = 0;
            const char* comboPreviewValue = depthTestFunc[depthTestFuncCurrentIdx];
            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Depth Testing Function", comboPreviewValue, ImGuiComboFlags_PopupAlignLeft))
            {
                for (int n = 0; n < IM_ARRAYSIZE(depthTestFunc); n++)
                {
                    const bool isSelected = (depthTestFuncCurrentIdx == n);
                    if (ImGui::Selectable(depthTestFunc[n], isSelected)) {
                        depthTestFuncCurrentIdx = n;
                        depthFunc = DEPTH_FUNC_VAL[n];
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    window.setVSyncEnabled(enableVSync);
    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, hideMouseCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    glPolygonMode(GL_FRONT_AND_BACK, drawWireFrame ? GL_LINE : GL_FILL);

    if (enableDepthTesting) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(depthFunc);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}