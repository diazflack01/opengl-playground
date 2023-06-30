add_executable(demo-phong-lighting main.cpp)
target_compile_features(demo-phong-lighting PRIVATE cxx_std_17)
target_link_libraries(demo-phong-lighting glad glfw fmt glm graphics utils)
