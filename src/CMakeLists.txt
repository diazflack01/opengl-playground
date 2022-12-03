add_executable(${PROJECT_NAME}
        main.cpp
        Shader.cpp
        Shader.hpp
        Camera.cpp
        Camera.hpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad stb glm)