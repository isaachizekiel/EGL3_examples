cmake_minimum_required(VERSION 3.16)
project(EGL3_examples C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

add_executable(EGL_linux src/egl_utils.c main.c)
