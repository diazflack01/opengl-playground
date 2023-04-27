add_executable(hello-triangle main.cpp)
target_compile_features(hello-triangle PRIVATE cxx_std_17)
target_link_libraries(hello-triangle glad glfw fmt glm graphics)
