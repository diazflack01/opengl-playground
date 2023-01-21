file(GLOB SRCS src/*.cpp)
message(${SRCS})
add_library(imgui STATIC ${SRCS})

target_include_directories(imgui PUBLIC include/imgui)

target_link_libraries(imgui glfw)