#include <fmt/core.h>

#include <graphics/WindowManager.hpp>
#include <graphics/Text2D.hpp>

const float SCREEN_WIDTH = 800.0f * 1.5;
const float SCREEN_HEIGTH = 600.0f * 1.5;
const auto WINDOW_TITLE = APP_NAME;


int main(int argc, char** argv) {
    fmt::println("main ()");

    WindowManager windowManager{SCREEN_WIDTH, SCREEN_HEIGTH, WINDOW_TITLE};
    graphics::Text2D text2D{windowManager};

    while (!windowManager.isCloseRequested()) {
        windowManager.update();

        glClearColor(.1f, 0.3f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        text2D.render("Hello World!", 0, 0);
        text2D.render("Django unchained", 300, 300, 0.5);

        windowManager.swapBuffers();
    }

    glfwTerminate();
    return 0;
}