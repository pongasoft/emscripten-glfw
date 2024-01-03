cmake_minimum_required(VERSION 3.5)

project(emscripten_glfw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(target "glfw3")

set(glfw3_headers
    src/cpp/emscripten/glfw3/Config.h
    src/cpp/emscripten/glfw3/Context.h
    src/cpp/emscripten/glfw3/Cursor.h
    src/cpp/emscripten/glfw3/ErrorHandler.h
    src/cpp/emscripten/glfw3/Events.h
    src/cpp/emscripten/glfw3/Joystick.h
    src/cpp/emscripten/glfw3/Keyboard.h
    src/cpp/emscripten/glfw3/KeyboardMapping.h
    src/cpp/emscripten/glfw3/Monitor.h
    src/cpp/emscripten/glfw3/Mouse.h
    src/cpp/emscripten/glfw3/Object.h
    src/cpp/emscripten/glfw3/Types.h
    src/cpp/emscripten/glfw3/Window.h
)

set(glfw3_sources
    src/cpp/glfw3.cpp
    src/cpp/emscripten/glfw3/Context.cpp
    src/cpp/emscripten/glfw3/ErrorHandler.cpp
    src/cpp/emscripten/glfw3/Joystick.cpp
    src/cpp/emscripten/glfw3/Keyboard.cpp
    src/cpp/emscripten/glfw3/Window.cpp
)

add_library(${target} ${glfw3_sources})
target_sources(${target} PUBLIC
    FILE_SET headers TYPE HEADERS BASE_DIRS src/cpp/emscripten/glfw3 FILES ${glfw3_headers}
)

target_include_directories(${target} PUBLIC external)
target_link_options(${target} PUBLIC "-lGL" "--js-library" "${CMAKE_CURRENT_LIST_DIR}/src/js/lib_emscripten_glfw3.js")

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  add_subdirectory("test/client" EXCLUDE_FROM_ALL)
endif()
