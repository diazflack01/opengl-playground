cmake_minimum_required(VERSION 3.16)

project(opengl-playground)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/stb)

add_subdirectory(src)