cmake_minimum_required(VERSION 3.10)

project(emscripten_glfw_example_resizable_container_with_handle LANGUAGES CXX)

if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "This example must be compiled with emscripten")
endif()

set(target "example-resizable-container-with-handle")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASSERTIONS=1 -s WASM=1 -s SAFE_HEAP=1 --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell.html")

add_executable(${target} main.cpp)
set_target_properties(${target} PROPERTIES OUTPUT_NAME "main")
set_target_properties(${target} PROPERTIES SUFFIX ".html")
target_link_libraries(${target} PRIVATE glfw3)
