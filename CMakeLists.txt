cmake_minimum_required(VERSION 3.0)

project(emscripten_glfw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(target "glfw3")

set(glfw3_sources
    src/cpp/glfw3.cpp
    src/cpp/emscripten/glfw3/Context.h
    src/cpp/emscripten/glfw3/Context.cpp
)

add_library(${target} ${glfw3_sources})

target_include_directories(${target} PUBLIC external)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  add_subdirectory("test/client" EXCLUDE_FROM_ALL)
endif()
