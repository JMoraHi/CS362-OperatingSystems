# Required by cmake, some kind of minimum version
cmake_minimum_required(VERSION 3.10)
project(PROJ02 C)

# Set the C standard to latest C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

# Make the compile_commands.json file for VSCode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include Warnings and debugging flags
add_compile_options(-Wall -g)

add_executable(simfs simfs.c cifs.c)
add_executable(mkfs.cifs cifs.c mkfs.c)
add_executable(fsck.cifs cifs.c fsck.c)