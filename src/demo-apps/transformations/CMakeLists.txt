add_executable(demo-transformations main.cpp)
target_compile_features(demo-transformations PRIVATE cxx_std_17)
target_link_libraries(demo-transformations glad glfw fmt glm graphics utils)
