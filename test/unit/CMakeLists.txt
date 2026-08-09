cmake_minimum_required(VERSION 3.10)

project(emscripten_glfw_unit_tests LANGUAGES CXX)

if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "This project must be compiled with emscripten")
endif()

set(target "unit-tests")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --emrun -fwasm-exceptions -s ASSERTIONS=1 -s WASM=1 -s SAFE_HEAP=1")

add_executable(${target} main.cpp)
set_target_properties(${target} PROPERTIES OUTPUT_NAME "index")
set_target_properties(${target} PROPERTIES SUFFIX ".html")
target_link_libraries(${target} PRIVATE glfw3)
target_compile_options(${target} PRIVATE "-fwasm-exceptions")

# adding header files from main project since this is about unit testing it
target_include_directories(${target} PUBLIC "../../src/cpp")

find_package(Python3 COMPONENTS Interpreter)

if(${Python3_EXECUTABLE} STREQUAL "Python3_EXECUTABLE-NOTFOUND")
  set(Python3_EXECUTABLE "python")
  message(STATUS "python library not found => using \"${Python3_EXECUTABLE}\" as the executable (make sure it is in the PATH)")
endif()

add_custom_target(run-unit-tests
    COMMAND "${Python3_EXECUTABLE}" "${EMSCRIPTEN_ROOT_PATH}/emrun.py" "--verbose" "--browser=firefox" "${CMAKE_CURRENT_BINARY_DIR}/index.html"
    DEPENDS ${target}
)
