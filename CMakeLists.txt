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

# todo: use target_include_directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(glb-lib-common OBJECT src/glb_output.c)
add_library(glb-lib-json-converter OBJECT src/json-converter/glb_json_to_gsl.c)

add_library(glb-lib_static STATIC
	$<TARGET_OBJECTS:glb-lib-common>
	$<TARGET_OBJECTS:glb-lib-json-converter>)
add_library(glb-lib SHARED
	    $<TARGET_OBJECTS:glb-lib-common>
	    $<TARGET_OBJECTS:glb-lib-json-converter>)
install(TARGETS glb-lib DESTINATION usr/lib)

add_executable(json2gsl tools/json2gsl.c)
target_link_libraries(json2gsl PUBLIC glb-lib)
install(TARGETS json2gsl DESTINATION usr/lib)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/options")
