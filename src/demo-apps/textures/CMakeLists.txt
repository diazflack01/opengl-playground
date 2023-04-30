add_executable(demo-textures main.cpp)
target_compile_features(demo-textures PRIVATE cxx_std_17)
target_link_libraries(demo-textures glad glfw fmt glm graphics utils)
