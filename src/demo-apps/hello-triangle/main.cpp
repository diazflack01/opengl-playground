#include <iostream>
#include <vector>

#include <glad/glad.h> // needs to be included before glfw
#include <GLFW/glfw3.h>

#include <fmt/core.h>

const float SCREEN_WIDTH = 800.0f * 1.5;
const float SCREEN_HEIGTH = 600.0f * 1.5;
const auto WINDOW_TITLE = "hello-triangle";

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processKeyboardInputs(GLFWwindow* window);

int main(int argc, char** argv) {
    fmt::println("main ()");

    glfwInit();
    // same as generated glad library
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGTH, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr) {
        fmt::println("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // event callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fmt::println("Failed to initialize GLAD");
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGTH);

    // vertices will be used in NDC which is (-1,-1) bottom-left to (1,1) top-right with origin at (0,0)
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
    };

    // create VAO, VBO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // bind VAO first, so all VBO buffer calls will be recorded
    glBindVertexArray(VAO);
    // bind buffer & set data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set data for vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind VAO
    glBindVertexArray(0);

    const char* vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "\n"
                                       "void main()\n"
                                       "{\n"
                                       "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\0";

    auto createShader = [](int shaderType, const char* shaderSrcCode){
        const unsigned int shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderSrcCode, NULL);
        glCompileShader(shader);

        int  success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            const auto errorMsg = fmt::format("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", infoLog);
            fmt::println(errorMsg);
            throw std::runtime_error(errorMsg);
        }
        return shader;
    };

    auto createShaderProgram = [](const std::vector<unsigned int> shaders){
        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();

        for (const auto& shader : shaders) {
            glAttachShader(shaderProgram, shader);
        }

        glLinkProgram(shaderProgram);

        int  success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
            const auto errorMsg = fmt::format("ERROR::SHADER::PROGRAM::LINK_FAILED\n{}", infoLog);
            fmt::println(errorMsg);
            throw std::runtime_error(errorMsg);
        }
        return shaderProgram;
    };

    // create shader program
    const auto vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    const auto fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    const auto shaderProgram = createShaderProgram({vertexShader, fragmentShader});

    // can delete vertex and fragment shader after program is created
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        processKeyboardInputs(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use program
        glUseProgram(shaderProgram);

        // bind VAO which has information about VBO data that we can draw
        glBindVertexArray(VAO);

        // draw triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

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
}