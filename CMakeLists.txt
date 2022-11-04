cmake_minimum_required(VERSION 3.16)
project(EGL3_examples C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(OpenGL REQUIRED)
# find_package(EGL REQUIRED)
# find_package(PkgConfig QUIET)
# pkg_check_modules(PKG_EGL QUIET egl)

find_package(X11 COMPONENTS X11)

include_directories(include)

add_executable(EGL_linux src/egl_utils.c src/x11_main.c)
target_link_libraries(EGL_linux PRIVATE
  -lX11 
  OpenGL::EGL )

