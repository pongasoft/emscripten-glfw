cmake_minimum_required(VERSION 3.5)

project(emscripten_glfw_example_pthread LANGUAGES CXX)

if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "This example must be compiled with emscripten")
endif()

set(target "example-pthread")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -sEXIT_RUNTIME=1 -sPROXY_TO_PTHREAD -s SAFE_HEAP=1 -s ASSERTIONS=1 -s WASM=1 --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell.html")

add_executable(${target} main.cpp)
target_compile_options(${target} PUBLIC "-pthread")
set_target_properties(${target} PROPERTIES OUTPUT_NAME "main")
set_target_properties(${target} PROPERTIES SUFFIX ".html")
target_link_libraries(${target} PRIVATE glfw3_pthread)
