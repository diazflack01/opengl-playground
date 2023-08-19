add_executable(demo-camera main.cpp)
target_compile_features(demo-camera PRIVATE cxx_std_17)
target_link_libraries(demo-camera glad glfw fmt glm graphics utils)

file(COPY ${PROJECT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})