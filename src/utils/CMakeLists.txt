add_library(utils
        ScopedTimer.cpp
        Utils.cpp)
target_compile_features(utils PRIVATE cxx_std_17)
target_include_directories(utils PUBLIC include PRIVATE include/utils)
target_link_libraries(utils glad stb fmt)
