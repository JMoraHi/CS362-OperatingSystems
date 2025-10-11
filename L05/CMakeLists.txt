# Required by cmake, some kind of minimum version
cmake_minimum_required(VERSION 3.10)
project(L05 C)

# Set the C standard to latest C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include Warnings and debugging flags
add_compile_options(-Wall -g)

# List of source files
set(SOURCES
    matrixMult.c
)

find_package(Threads)
# Loop through sources and create executables
foreach(SRC ${SOURCES})
    # Get the filename without extension
    get_filename_component(EXEC_NAME ${SRC} NAME_WE)
    add_executable(${EXEC_NAME} ${SRC})
    target_link_libraries(${EXEC_NAME} ${CMAKE_THREAD_LIBS_INIT})
endforeach()