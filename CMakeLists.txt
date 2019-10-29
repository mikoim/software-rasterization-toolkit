cmake_minimum_required(VERSION 3.14)
project(render C)

set(CMAKE_C_STANDARD 99)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-Og -g3")
set(CMAKE_C_FLAGS_RELEASE "-O3 -g0 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

add_library(bitmap bitmap.c bitmap.h)

add_library(matrix matrix.c matrix.h)
target_link_libraries(matrix m)

add_library(vector vector.c vector.h)
target_link_libraries(vector m)

add_library(polygon polygon.c polygon.h)
target_link_libraries(polygon vector)

add_library(camera camera.c camera.h)
target_link_libraries(camera matrix vector)

add_library(transformer transformer.c transformer.h)
target_link_libraries(transformer matrix vector)

add_library(rasterizer rasterizer.c rasterizer.h world.c world.h)
target_link_libraries(rasterizer bitmap polygon camera transformer)

add_executable(matrix_test matrix_test.c)
target_link_libraries(matrix_test matrix)

add_executable(vector_test vector_test.c)
target_link_libraries(vector_test vector)

add_executable(example_hue_scale example_hue_scale.c)
target_link_libraries(example_hue_scale bitmap)

add_executable(example_triangle example_triangle.c)
target_link_libraries(example_triangle bitmap vector)

add_executable(example_polygon example_polygon.c)
target_link_libraries(example_polygon rasterizer)

add_executable(example_polygon_rotation example_polygon_rotation)
target_link_libraries(example_polygon_rotation rasterizer)

add_executable(example_polygon_depth example_polygon_depth)
target_link_libraries(example_polygon_depth rasterizer)

add_executable(example_polygon_shading example_polygon_shading.c)
target_link_libraries(example_polygon_shading rasterizer)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPOResult OUTPUT IPOOutput)
    if (IPOResult)
        message(STATUS "IPO is supported")
        set_property(TARGET bitmap PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET matrix PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET vector PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET matrix_test PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET vector_test PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET example_hue_scale PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET example_triangle PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
        set_property(TARGET example_polygon PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        message(WARNING "IPO is not supported: ${IPOOutput}")
    endif ()
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    endif()
endif ()
