cmake_minimum_required(VERSION 3.10)
project(cpuScheduler C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-g -O0 -D_DEBUG -Wall")
set(SOURCE_FILES cpuScheduler.c processQueue.c)

add_executable(cpuScheduler ${SOURCE_FILES})