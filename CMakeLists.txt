cmake_minimum_required(VERSION 3.0)

project(emscripten_glfw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(target "glfw3")

add_library(${target} src/glfw3.cpp)

target_include_directories(${target} PUBLIC external)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  add_subdirectory("test/client" EXCLUDE_FROM_ALL)
endif()
