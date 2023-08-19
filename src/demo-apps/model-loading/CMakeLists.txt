add_executable(demo-model-loading main.cpp)
target_compile_features(demo-model-loading PRIVATE cxx_std_17)
target_link_libraries(demo-model-loading glad glfw fmt glm graphics assimp)

file(COPY ${PROJECT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})