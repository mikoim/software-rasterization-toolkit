cmake_minimum_required(VERSION 3.14)
project(render C)

set(CMAKE_C_STANDARD 99)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-Og -g3")
set(CMAKE_C_FLAGS_RELEASE "-O3 -g0 -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

add_library(bitmap bitmap.c bitmap.h)

add_library(matrix matrix.c matrix.h)
target_link_libraries(matrix m)

add_library(vector vector.c vector.h)
target_link_libraries(vector m)

add_executable(vector_test vector_test.c)
target_link_libraries(vector_test vector)

add_executable(matrix_test matrix_test.c)
target_link_libraries(matrix_test matrix)

add_executable(example_hue_scale example_hue_scale.c)
target_link_libraries(example_hue_scale bitmap)

add_executable(example_triangle example_triangle.c)
target_link_libraries(example_triangle bitmap vector)

#add_executable(render main.c)
target_link_libraries(example_polygon bitmap matrix vector)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPOResult OUTPUT IPOOutput)
    if (IPOResult)
        message(STATUS "IPO is supported")
        set_property(TARGET bitmap PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET matrix PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET example_hue_scale PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET example_triangle PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET example_polygon_2d PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        message(WARNING "IPO is not supported: ${IPOOutput}")
    endif ()
endif ()
