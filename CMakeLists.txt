cmake_minimum_required(VERSION 3.5)

project(emscripten_glfw LANGUAGES CXX)

if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "This library must be compiled with emscripten")
endif()

set(CMAKE_CXX_STANDARD 17)

# Use this CMake option to disable joystick support
option(EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK "Disable joystick support" OFF)

# Use this CMake option to disable multi window support
option(EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT "Disable multi window support" OFF)

set(target "glfw3")

set(glfw3_headers
    src/cpp/emscripten/glfw3/Config.h
    src/cpp/emscripten/glfw3/Context.h
    src/cpp/emscripten/glfw3/Cursor.h
    src/cpp/emscripten/glfw3/ErrorHandler.h
    src/cpp/emscripten/glfw3/Events.h
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
    src/cpp/emscripten/glfw3/Keyboard.cpp
    src/cpp/emscripten/glfw3/Window.cpp
)

if(NOT EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK)
  list(APPEND glfw3_headers src/cpp/emscripten/glfw3/Joystick.h)
  list(APPEND glfw3_sources src/cpp/emscripten/glfw3/Joystick.cpp)
endif()

add_library(${target} ${glfw3_sources})
target_sources(${target} PUBLIC
    FILE_SET headers TYPE HEADERS BASE_DIRS src/cpp/emscripten/glfw3 FILES ${glfw3_headers}
)

target_include_directories(${target} PUBLIC external include)
target_compile_definitions(${target} PUBLIC
    $<$<CONFIG:Release>:EMSCRIPTEN_GLFW3_DISABLE_WARNING>
    $<$<BOOL:${EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK}>:EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK>
    $<$<BOOL:${EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT}>:EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT>
)
target_link_options(${target} PUBLIC "-lGL" "--js-library" "${CMAKE_CURRENT_LIST_DIR}/src/js/lib_emscripten_glfw3.js")

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  add_subdirectory("test/demo" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_asyncify" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_hi_dpi" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_minimal" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_resizable_container" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_resizable_container_with_handle" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_resizable_full_window" EXCLUDE_FROM_ALL)
  add_custom_target("examples-all"
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/examples/index.html ${CMAKE_CURRENT_BINARY_DIR}
      DEPENDS
      "demo"
      "example-asyncify"
      "example-hi-dpi"
      "example-minimal"
      "example-resizable-container"
      "example-resizable-container-with-handle"
      "example-resizable-full-window"
  )
endif()
