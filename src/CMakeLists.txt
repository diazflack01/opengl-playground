add_executable(${PROJECT_NAME}
        main.cpp
        Shader.cpp
        Shader.hpp
        Camera.cpp
        Camera.hpp
        Mesh.cpp
        Mesh.hpp
        Model.cpp
        Model.hpp
        Utils.cpp
        Utils.hpp
        ScopedTimer.cpp
        ScopedTimer.hpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad stb glm assimp imgui)