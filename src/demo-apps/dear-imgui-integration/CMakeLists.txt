
set(App demo-dear-imgui-integration)
add_executable(${App} main.cpp)
target_compile_features(${App} PRIVATE cxx_std_17)
target_link_libraries(${App} glad glfw fmt glm graphics utils imgui)
target_compile_definitions(${App} PRIVATE APP_NAME="${App}")

file(COPY ${PROJECT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})