cmake_minimum_required(VERSION 3.16)

project(opengl-playground)

# for IDE parsing
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# this-party libs
add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/stb)
add_subdirectory(external/glm)
add_subdirectory(external/assimp)
add_subdirectory(external/imgui-glfw)
add_subdirectory(external/fmt)
add_subdirectory(external/freetype)

add_subdirectory(src)
