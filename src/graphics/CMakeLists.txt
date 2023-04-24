set(LIB_NAME "graphics")

add_library(${LIB_NAME}
        Shader.cpp
        Camera.cpp
        Mesh.cpp
        Model.cpp
        Bone.cpp
        Animation.cpp
        Animator.cpp)

target_compile_features(${LIB_NAME} PRIVATE cxx_std_17)
target_link_libraries(${LIB_NAME} PRIVATE assimp glm glad utils)
target_include_directories(${LIB_NAME} PUBLIC include PRIVATE include/graphics)