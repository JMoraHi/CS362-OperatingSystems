project(sub C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-g -O0 -D_DEBUG -Wall")
set(SOURCE_FILES sub.c)

add_executable(sub ${SOURCE_FILES})