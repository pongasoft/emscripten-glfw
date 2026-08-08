# This script runs at build/task execution time.
# Required variables passed via -D:
#   ARCHIVE_FILES_LIST (path to archive.txt)
#   SOURCE_DIR         (CMAKE_CURRENT_SOURCE_DIR)
#   BUILD_DIR          (ARCHIVE_BUILD_DIR)
#   emscripten-glfw_VERSION (or any other @VAR@ used in configured files)

message(STATUS "Processing archive files")

file(STRINGS "${ARCHIVE_FILES_LIST}" ARCHIVE_FILES)

foreach(FILE ${ARCHIVE_FILES})
  set(ARCHIVE_FILE "${BUILD_DIR}/${FILE}")
  get_filename_component(FILE_DIR "${ARCHIVE_FILE}" DIRECTORY)
  file(MAKE_DIRECTORY "${FILE_DIR}")

  # This configure_file runs at build time, so modifying the source files
  # will NOT trigger a CMake configure/generate step.
  configure_file("${SOURCE_DIR}/${FILE}" "${ARCHIVE_FILE}" @ONLY)
endforeach()