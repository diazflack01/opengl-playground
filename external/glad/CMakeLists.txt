# TODO: Make this header only library
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include)