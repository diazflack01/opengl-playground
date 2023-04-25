add_executable(model-loading main.cpp)
target_compile_features(model-loading PRIVATE cxx_std_17)
target_link_libraries(model-loading glad glfw fmt glm graphics)
