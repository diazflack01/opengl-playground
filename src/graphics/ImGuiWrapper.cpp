#include "ImGuiWrapper.hpp"
#include "WindowManager.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace graphics {
ImGuiWrapper::ImGuiWrapper(WindowManager &windowManager) : mWindowManager{windowManager} {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(mWindowManager.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiWrapper::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    update();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    updateWindowManager();
}

void ImGuiWrapper::update() {
    if (mShowDemoWindow) {
        ImGui::ShowDemoWindow(&mShowDemoWindow);
    }

    ImGui::Begin("Settings");
    ImGui::Checkbox("ImGui Demo Window", &mShowDemoWindow);
    ImGui::Checkbox("V-Sync", &mEnableVSync);
    ImGui::Checkbox("Hide Mouse Cursor", &mHideCursor);
    ImGui::Checkbox("Wireframe drawing", &mDrawWireFrame);
    ImGui::Checkbox("Depth testing", &mEnableDepthTesting);
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void ImGuiWrapper::updateWindowManager() const {
    mWindowManager.setVSyncEnabled(mEnableVSync);
    glfwSetInputMode(mWindowManager.getWindow(), GLFW_CURSOR, mHideCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    glPolygonMode(GL_FRONT_AND_BACK, mDrawWireFrame ? GL_LINE : GL_FILL);

    if (mEnableDepthTesting) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}
}
