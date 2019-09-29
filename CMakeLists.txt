cmake_minimum_required(VERSION 3.14)
project(render C)

set(CMAKE_C_STANDARD 99)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -g0 -s -march=native")

add_library(bitmap bitmap.c bitmap.h)
add_library(matrix matrix.c matrix.h)
target_link_libraries(matrix m)

add_executable(example_hue_scale example_hue_scale.c)
target_link_libraries(example_hue_scale bitmap)

add_executable(example_triangle example_triangle.c)
target_link_libraries(example_triangle bitmap matrix)

#add_executable(render main.c)
#target_link_libraries(render bitmap matrix)
