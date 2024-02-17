### What does it do? 

The purpose of this example is to demonstrate how to make the canvas resizable with another container (a
surrounding `div`) driving its size. The container width is proportional to the size of the window and so as the
window gets resized so does the `div` and so does the canvas.

### Building instructions

Using `emcc` from the command line (make sure it is in your path)

```sh
mkdir build
emcc --use-port=contrib.glfw3 --shell-file=shell.html main.cpp -o build/index.html
```

Using `CMake`:

```cmake
set(target "example-resizable-container")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --use-port=contrib.glfw3 --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell.html")
add_executable(${target} main.cpp)
set_target_properties(${target} PROPERTIES OUTPUT_NAME "index")
set_target_properties(${target} PROPERTIES SUFFIX ".html")
```

### Running instructions

```sh
# start a webserver on port 8000
python3 -m http.server -d build
```

Run in your browser: http://localhost:8000