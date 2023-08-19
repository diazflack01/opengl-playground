#include <iostream>
#include <vector>

#include <fmt/core.h>

#include <graphics/WindowManager.hpp>
#include <graphics/Shader.hpp>
#include <utils/Utils.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp> // glm::to_string

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGTH = 600.0f;
const auto WINDOW_TITLE = "demo-coordinate-system";
constexpr glm::mat4 IDENTITY_MATRIX{1.0};


int main(int argc, char** argv) {
    fmt::println("main ()");

    WindowManager windowManager{SCREEN_WIDTH, SCREEN_HEIGTH, WINDOW_TITLE};

    // vertices will be used in NDC which is (-1,-1) bottom-left to (1,1) top-right with origin at (0,0)
    float vertices[] = {
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

    const std::vector<glm::vec3> cubePositions{
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind VAO
    glBindVertexArray(0);

    Shader shader{"resources/shader/demo_coordinate_system.vert", "resources/shader/demo_coordinate_system.frag"};
    const auto textureAwesomeFace = loadTexture("resources/texture/awesomeface.png");
    const auto textureWoodenContainer = loadTexture("resources/texture/wooden_container.jpg");
    const auto texturePepefrog = loadTexture("resources/texture/pepe-frog.jpg");

    // draw wireframe. Default is GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // enable depth/z-buffer testing
    glEnable(GL_DEPTH_TEST);

    /*!
     * Coordinate system:
     *  - model: matrix to transform to world space coordinates
     *  - view: matrix to transform to camera/eye space coordinates
     *  - projection:
     *      - matrix to generate vec4 which clip-space can be computed from by performing perspective division
     *      - perspective division is simply dividing the `w` component to `x`, `y`, `z` component
     *      - clip-space is (-1.0, 1.0) range, any coordinate outside this is discarded after perspective division
     *      - perspective division is performed at/after(?) vertex shader by OpenGL
     *      - clip-space will be converted to screen-space/viewport coordinates which is passed to fragment shader
     *
     * Below is an illustration where `Z` coordinate exceeds the far plane 100.0f. The log prints will show the vec4
     * generated from glm::perspective method and what will be the output coordinate of clip space.
     */
    {
        // vertex position
        const glm::vec3 vertexPos{0.0, 0.0, 0.0};
        // projection matrix with far plane 100.f
        const auto projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH/SCREEN_HEIGTH, 0.1f, 100.0f);
        // model matrix is translated to -100.f to position it exactly at the far plane
        auto model = glm::translate(IDENTITY_MATRIX, glm::vec3{0.0, 0.0, -100.0});
        // view matrix is at origin so far plane is easy to test
        const auto view = glm::translate(IDENTITY_MATRIX, glm::vec3{0.0, 0.0, 0.0});
        // z = 100.0 is within the far plane, thus after perspective division `z` coordinate is still within clip-space
        auto mvp = projection * view * model * glm::vec4{vertexPos, 1.0};
        fmt::println("MVP output with z -100 [{}]\t--> perspective division x:{}, y:{}, z:{}", glm::to_string(mvp), mvp.x/mvp.w, mvp.y/mvp.w, mvp.z/mvp.w);

        // z = 100.01 is beyond the far plane, after perspective division it's z coordinate is expected to be outside clip-space
        model = glm::translate(IDENTITY_MATRIX, glm::vec3{0.0, 0.0, -100.01});
        mvp = projection * view * model * glm::vec4{vertexPos, 1.0};
        fmt::println("MVP output with z -100.01 [{}]\t--> perspective division x:{}, y:{}, z:{}", glm::to_string(mvp), mvp.x/mvp.w, mvp.y/mvp.w, mvp.z/mvp.w);
    }

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
        shader.setInt("texture0", 4); // 4 - GL_TEXTURE4
        shader.setInt("texture1", 8); // 8 - GL_TEXTURE8

        // bind VAO then draw the cube
        glBindVertexArray(VAO);

        // MVP
        const auto view = glm::translate(IDENTITY_MATRIX, glm::vec3{0.0, 0.0, -3.0});
        const auto projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH/SCREEN_HEIGTH, 0.1f, 100.0f);

        // set uniform values
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // draw cubes with different model matrix
        for (auto idx = 0; idx < cubePositions.size(); idx++) {
            auto model = glm::translate(IDENTITY_MATRIX, cubePositions[idx]);
            const float angle = 20.0f * idx;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        windowManager.swapBuffers();
    }

    glfwTerminate();
    return 0;
}