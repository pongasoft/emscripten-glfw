cmake_minimum_required(VERSION 3.10)

project(emscripten_glfw_demo LANGUAGES CXX)

set(target "demo")

# the linker flag -sMAX_WEBGL_VERSION=2 is used because due to Emscripten internals,
# the GLFW_CONTEXT_VERSION_MAJOR is ignored. When using the port, the flag is
# automatically set so there is no need to set it.
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sMAX_WEBGL_VERSION=2 -sNO_DISABLE_EXCEPTION_CATCHING -s ASSERTIONS=1 -s WASM=1 -s SAFE_HEAP=1")

add_executable(${target} src/main.cpp src/Triangle.cpp)
set_target_properties(${target} PROPERTIES OUTPUT_NAME "main")
target_link_libraries(${target} PRIVATE glfw3)

# Copy (and processes) shell.html
set(SHELL_SRC "${CMAKE_CURRENT_LIST_DIR}/shell.html")
set(SHELL_DST "${CMAKE_CURRENT_BINARY_DIR}/main.html")

add_custom_command(
    OUTPUT ${SHELL_DST}
    COMMAND ${CMAKE_COMMAND} -D SRC=${SHELL_SRC} -D DST=${SHELL_DST} -P ${CMAKE_CURRENT_LIST_DIR}/CopyResource.cmake
    DEPENDS ${SHELL_SRC}
)
add_custom_target(copy_shell_html DEPENDS ${SHELL_DST})

add_dependencies(${target} copy_shell_html)

# To build using emcc:
# mkdir /tmp/build
# cp shell.html /tmp/build/index.html
# emcc --use-port=../../port/emscripten-glfw3.py src/main.cpp src/Triangle.cpp -o /tmp/build/main.js