# TODO: Clean this target
add_library(testing-data
        TestingData.cpp
        TestingData.hpp)
target_compile_features(testing-data PRIVATE cxx_std_17)
target_link_libraries(testing-data glm)

# dependencies
add_subdirectory(utils)
add_subdirectory(graphics)

# main executable
add_executable(${PROJECT_NAME}
        main.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad stb glm assimp imgui fmt graphics utils testing-data)

add_subdirectory(demo-apps)