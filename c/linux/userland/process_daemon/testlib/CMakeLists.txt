cmake_minimum_required(VERSION 3.0)
project(test C)

set(CMAKE_C_STANDARD 99)

add_library(test SHARED test.c)