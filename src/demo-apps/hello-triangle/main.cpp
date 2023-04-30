#include <iostream>
#include <vector>

#include <fmt/core.h>

#include <graphics/WindowManager.hpp>

const float SCREEN_WIDTH = 800.0f * 1.5;
const float SCREEN_HEIGTH = 600.0f * 1.5;
const auto WINDOW_TITLE = "hello-triangle";


int main(int argc, char** argv) {
    fmt::println("main ()");

    WindowManager windowManager{SCREEN_WIDTH, SCREEN_HEIGTH, WINDOW_TITLE};

    // vertices will be used in NDC which is (-1,-1) bottom-left to (1,1) top-right with origin at (0,0)
    float vertices[] = {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
    };

    // create VAO, VBO, EBO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // bind VAO first, so all VBO, EBO buffer calls will be recorded
    glBindVertexArray(VAO);
    // bind buffer & set data - VBO, EBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set VAO data layout attributes for vertex shader usage
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

    // draw wireframe. Default is GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!windowManager.isCloseRequested()) {
        windowManager.update();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use program
        glUseProgram(shaderProgram);

        // bind VAO which has information about VBO data that we can draw
        glBindVertexArray(VAO);

        // draw triangle by going through VBO data
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // draw triangle using indices to go through VBO data
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        windowManager.swapBuffers();
    }

    glfwTerminate();
    return 0;
}