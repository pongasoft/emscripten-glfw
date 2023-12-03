cmake_minimum_required(VERSION 3.0)

project(emscripten_glfw_client LANGUAGES C)

set(target "main")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --profiling-funcs -s ASSERTIONS=1 -s WASM=1 -s SAFE_HEAP=1 --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell.html")

set(CMAKE_EXECUTABLE_SUFFIX ".html")

add_executable(${target} src/main.c)
target_link_libraries(${target} PRIVATE glfw3)
