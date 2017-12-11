cmake_minimum_required(VERSION 3.2)
project(glb-lib C)
find_package(PkgConfig)

set(CMAKE_C_STANDARD 11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c11 -fPIC")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -Werror")
set(CMAKE_C_FLAGS_PACKAGE "${CMAKE_C_FLAGS_RELEASE}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/glb-lib")

add_library(glb-lib-list OBJECT src/list/list.c)
add_library(glb-lib-output OBJECT src/glb_output.c)

add_library(glb-lib SHARED
    $<TARGET_OBJECTS:glb-lib-list>
    $<TARGET_OBJECTS:glb-lib-output>)
