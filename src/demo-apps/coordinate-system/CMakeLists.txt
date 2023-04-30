add_executable(demo-coordinate-system main.cpp)
target_compile_features(demo-coordinate-system PRIVATE cxx_std_17)
target_link_libraries(demo-coordinate-system glad glfw fmt glm graphics utils)
