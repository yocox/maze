cmake_minimum_required(VERSION 3.16)
project(MazeGenerator)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Draw library
add_library(draw STATIC draw.cc)
target_compile_options(draw PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_include_directories(draw PUBLIC C:/lib/CImg-3.1.0_pre040122)

# Main
add_executable(a test.cc)
target_link_libraries(a PUBLIC draw)