cmake_minimum_required(VERSION 3.10)

set(emscripten-glfw_RELEASE_YEAR  "2025")
set(emscripten-glfw_RELEASE_MONTH "02"  )
set(emscripten-glfw_RELEASE_DAY   "02"  )

set(emscripten-glfw_GLFW_VERSION "3.4.0")

set(emscripten-glfw_PORT_VERSION "${emscripten-glfw_RELEASE_YEAR}${emscripten-glfw_RELEASE_MONTH}${emscripten-glfw_RELEASE_DAY}")
set(emscripten-glfw_VERSION "${emscripten-glfw_GLFW_VERSION}.${emscripten-glfw_PORT_VERSION}")
set(emscripten-glfw_VERSION_AS_NUMBER "3'4'0'${emscripten-glfw_PORT_VERSION}")
set(emscripten-glfw_RELEASE_DATE "${emscripten-glfw_RELEASE_YEAR}-${emscripten-glfw_RELEASE_MONTH}-${emscripten-glfw_RELEASE_DAY}")

project(emscripten-glfw VERSION "${emscripten-glfw_VERSION}" LANGUAGES CXX)

if(NOT EMSCRIPTEN)
  message(FATAL_ERROR "This library must be compiled with emscripten")
endif()

set(CMAKE_CXX_STANDARD 17)

# Use this CMake option to disable joystick support
option(EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK "Disable joystick support" OFF)

# Use this CMake option to disable multi window support
option(EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT "Disable multi window support" OFF)

set(glfw3_headers
    src/cpp/emscripten/glfw3/Clipboard.h
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
    src/cpp/emscripten/glfw3/Clipboard.cpp
    src/cpp/emscripten/glfw3/Context.cpp
    src/cpp/emscripten/glfw3/ErrorHandler.cpp
    src/cpp/emscripten/glfw3/Keyboard.cpp
    src/cpp/emscripten/glfw3/Window.cpp
)

if(NOT EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK)
  list(APPEND glfw3_headers src/cpp/emscripten/glfw3/Joystick.h)
  list(APPEND glfw3_sources src/cpp/emscripten/glfw3/Joystick.cpp)
endif()

# This is very convoluted, but the only way I found for the zip file
# checksum to not change every time CMake is rerun
configure_file("${CMAKE_CURRENT_LIST_DIR}/version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/emscripten_glfw3_version.h")
set(VERSION_FILE "${CMAKE_CURRENT_LIST_DIR}/include/GLFW/emscripten_glfw3_version.h")
file(SHA256 "${CMAKE_CURRENT_BINARY_DIR}/emscripten_glfw3_version.h" NEW_VERSION_HASH)
file(SHA256 "${VERSION_FILE}" OLD_VERSION_HASH)
if(NOT ${NEW_VERSION_HASH} STREQUAL ${OLD_VERSION_HASH})
  configure_file("${CMAKE_CURRENT_BINARY_DIR}/emscripten_glfw3_version.h" "${VERSION_FILE}")
endif()

macro(add_glfw_lib LIBRARY_NAME)
  add_library(${LIBRARY_NAME} ${glfw3_sources})
  target_sources(${LIBRARY_NAME} PUBLIC
      FILE_SET headers TYPE HEADERS BASE_DIRS src/cpp/emscripten/glfw3 FILES ${glfw3_headers}
  )

  target_include_directories(${LIBRARY_NAME} PUBLIC external include)
  target_compile_definitions(${LIBRARY_NAME} PUBLIC
      $<$<CONFIG:Release>:EMSCRIPTEN_GLFW3_DISABLE_WARNING>
      $<$<BOOL:${EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK}>:EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK>
      $<$<BOOL:${EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT}>:EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT>
  )
  target_link_options(${LIBRARY_NAME} PUBLIC "-lGL" "--js-library" "${CMAKE_CURRENT_LIST_DIR}/src/js/lib_emscripten_glfw3.js")
endmacro()

# main library
add_glfw_lib("glfw3")

# library compiled with -pthread
add_glfw_lib("glfw3_pthread")
target_compile_options(glfw3_pthread PUBLIC "-pthread")

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  ### create archive/emscripten-glfw3-${emscripten-glfw_VERSION}.zip for the port
  set(ARCHIVE_DIR "${CMAKE_CURRENT_BINARY_DIR}/archive")
  set(ARCHIVE_NAME "${ARCHIVE_DIR}/emscripten-glfw3-${emscripten-glfw_VERSION}.zip")
  add_custom_target("create-archive"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND ${CMAKE_COMMAND} -E remove_directory ${ARCHIVE_DIR}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${ARCHIVE_DIR}
      COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${ARCHIVE_NAME}" "--format=zip" "--mtime=${emscripten-glfw_RELEASE_DATE}" "--files-from=${CMAKE_CURRENT_LIST_DIR}/archive.txt"
      COMMAND ${CMAKE_COMMAND} -E sha512sum "${ARCHIVE_NAME}" > "${ARCHIVE_NAME}.sha512"
      COMMAND cat "${ARCHIVE_NAME}.sha512"
  )

  ### create a port pointing to archive
  add_custom_target("local-port"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMAND tools/create_local_port.py "${CMAKE_CURRENT_BINARY_DIR}" "${emscripten-glfw_VERSION}"
      DEPENDS "create-archive"
  )

  ### compile all the examples
  add_subdirectory("test/demo" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_asyncify" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_hi_dpi" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_minimal" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_offscreen_canvas" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_pthread" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_resizable_container" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_resizable_container_with_handle" EXCLUDE_FROM_ALL)
  add_subdirectory("examples/example_resizable_full_window" EXCLUDE_FROM_ALL)
  add_custom_target("examples-all"
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/examples/index.html" "${CMAKE_CURRENT_BINARY_DIR}"
      DEPENDS
      "demo"
      "example-asyncify"
      "example-hi-dpi"
      "example-minimal"
      "example-offscreen-canvas"
      "example-pthread"
      "example-resizable-container"
      "example-resizable-container-with-handle"
      "example-resizable-full-window"
  )

  ### create examples.zip for easy deploy
  set(EXAMPLES_BUILD_FILES "${CMAKE_CURRENT_BINARY_DIR}/examples_build_files.txt")
  set(EXAMPLES_ZIP_FILE "${CMAKE_CURRENT_BINARY_DIR}/examples.zip")
  add_custom_target("examples-zip"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
      COMMAND find . -name main.html > "${EXAMPLES_BUILD_FILES}"
      COMMAND find . -name main.js >> "${EXAMPLES_BUILD_FILES}"
      COMMAND find . -name main.wasm >> "${EXAMPLES_BUILD_FILES}"
      COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${EXAMPLES_ZIP_FILE}" "--format=zip" "--files-from=${EXAMPLES_BUILD_FILES}"
      DEPENDS "examples-all"
  )
endif()
