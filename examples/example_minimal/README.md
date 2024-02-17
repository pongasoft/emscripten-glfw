### What does it do? 

The purpose of this example is to be as minimal as possible: initializes glfw, create window, then destroy it and terminate glfw.
Uses the default shell that comes with emscripten.

### Building instructions

Using `emcc` from the command line (make sure it is in your path)

```sh
mkdir build
emcc --use-port=contrib.glfw3 main.cpp -o build/index.html
```

Using `CMake`:

```cmake
set(target "example-minimal")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --use-port=contrib.glfw3")
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