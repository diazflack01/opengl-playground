set(LIB_NAME "graphics")

add_library(${LIB_NAME}
        STATIC
        WindowManager.cpp
        Shader.cpp
        Camera.cpp
        Mesh.cpp
        Model.cpp
        Bone.cpp
        Animation.cpp
        Animator.cpp
        Mouse.cpp
        VertexBuffer.cpp
        VertexArray.cpp
        IndexBuffer.cpp
        Texture.cpp
        ImGuiWrapper.cpp
        Text2D.cpp)

target_compile_features(${LIB_NAME} PRIVATE cxx_std_17)
# PUBLIC linked libraries to avoid missing header (from glfw, glm, glad) when some graphics/*.hpp
# included are compiled by client code, i.e. WindowManager.hpp will error for #include <glad/glad.h>
# if glad was marked as PRIVATE
target_link_libraries(${LIB_NAME} PUBLIC glfw glm glad PRIVATE assimp utils imgui freetype fmt)
target_include_directories(${LIB_NAME} PUBLIC include PRIVATE include/graphics)