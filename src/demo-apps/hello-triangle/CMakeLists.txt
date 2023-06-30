add_executable(demo-hello-triangle main.cpp)
target_compile_features(demo-hello-triangle PRIVATE cxx_std_17)
target_link_libraries(demo-hello-triangle glad glfw fmt glm graphics)
