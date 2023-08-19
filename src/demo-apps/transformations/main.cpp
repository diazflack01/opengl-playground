#include <iostream>
#include <vector>

#include <fmt/core.h>

#include <graphics/WindowManager.hpp>
#include <graphics/Shader.hpp>
#include <utils/Utils.hpp>

#include <glm/gtc/matrix_transform.hpp>

const float SCREEN_WIDTH = 800.0f * 1.5;
const float SCREEN_HEIGTH = 600.0f * 1.5;
const auto WINDOW_TITLE = "demo-textures";


int main(int argc, char** argv) {
    fmt::println("main ()");

    WindowManager windowManager{SCREEN_WIDTH, SCREEN_HEIGTH, WINDOW_TITLE};

    // vertices will be used in NDC which is (-1,-1) bottom-left to (1,1) top-right with origin at (0,0)
    float vertices[] = {
            // positions          // colors           // texture coords
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind VAO
    glBindVertexArray(0);

    Shader shader{"resources/shader/demo_transformation.vert", "resources/shader/demo_transformation.frag"};
    const auto textureAwesomeFace = loadTexture("resources/texture/awesomeface.png");
    const auto textureWoodenContainer = loadTexture("resources/texture/wooden_container.jpg");
    const auto texturePepefrog = loadTexture("resources/texture/pepe-frog.jpg");

    // draw wireframe. Default is GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    while (!windowManager.isCloseRequested()) {
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
        shader.setInt("texture0", 0); // 0 - GL_TEXTURE0
        shader.setInt("texture1", 8); // 8 - GL_TEXTURE8

        // draw rectangle
        auto drawRectangle = [&VAO]{
            // bind VAO which has information about VBO data that we can draw
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        };

        // transform 1: rotate -> scale
        constexpr glm::mat4 IDENTITY_MATRIX{1.0};
        const auto scale = glm::scale(IDENTITY_MATRIX, glm::vec3{0.5, 0.5, 0.5});
        const auto rotate = glm::rotate(IDENTITY_MATRIX, glm::radians(45.0f), glm::vec3{0.0, 0.0, 1.0});
        const auto transform1 = scale * rotate;
        shader.setMat4("transform", transform1);
        drawRectangle();

        // rotate transform based from time
        const auto rotateOverTime = glm::rotate(IDENTITY_MATRIX, (float)glfwGetTime(), glm::vec3{0.0, 0.0, 1.0});
        const auto translate = glm::translate(IDENTITY_MATRIX, glm::vec3{.75, 0.0, 0.0});

        // transform 2: translate -> scale -> rotateOverTime
        //  - rotates around origin (0,0)
        const auto transform2 = scale * rotateOverTime * translate;
        shader.setMat4("transform", transform2);
        drawRectangle();

        // transform 3: rotateOverTime -> translate -> scale
        //  - rotates around the origin after translate
        const auto transform3 = glm::scale(scale, glm::vec3{0.5, 0.5, 0.5}) * translate * rotateOverTime;
        shader.setMat4("transform", transform3);
        drawRectangle();

        windowManager.swapBuffers();
    }

    glfwTerminate();
    return 0;
}