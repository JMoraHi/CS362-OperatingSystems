cmake_minimum_required(VERSION 3.10)
project(invertedDynamic C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall --pedantic -g -O0 -D_DEBUG")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_library(inverted SHARED src/inverted.c)

include_directories(inc)

add_executable(testInvertedImplicit src/testInvertedImplicit.c)
target_link_libraries(testInvertedImplicit inverted)

add_executable(testInvertedExplicit src/testInvertedExplicit.c)
target_link_libraries(testInvertedExplicit inverted ${CMAKE_DL_LIBS})

