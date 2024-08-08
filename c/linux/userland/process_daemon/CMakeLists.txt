cmake_minimum_required(VERSION 3.0)
project(CCPControl C)

set(CMAKE_C_STANDARD 99)

# set the output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin/lib)

add_executable(CCPControl main.c)
# -ldl
target_link_libraries(CCPControl dl)


add_subdirectory(testlib)
