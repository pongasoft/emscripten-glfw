cmake_minimum_required(VERSION 3.5)

project(emscripten_glfw_demo LANGUAGES CXX)

set(target "demo")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sMIN_WEBGL_VERSION=2 -sNO_DISABLE_EXCEPTION_CATCHING -s ASSERTIONS=1 -s WASM=1 -s SAFE_HEAP=1 --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell.html")

add_executable(${target} src/main.cpp src/Triangle.cpp)
set_target_properties(${target} PROPERTIES OUTPUT_NAME "main")
set_target_properties(${target} PROPERTIES SUFFIX ".html")
target_link_libraries(${target} PRIVATE glfw3)
