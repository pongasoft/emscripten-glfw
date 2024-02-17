### What does it do? 

The purpose of this example is to demonstrate how to use asyncify which allows the code to be written like you
would for a normal desktop application.

### Building instructions

Using `emcc` from the command line (make sure it is in your path)

```sh
mkdir build
emcc --use-port=contrib.glfw3 -sASYNCIFY --shell-file=shell.html main.cpp -o build/index.html
```

Using `CMake`:

```cmake
set(target "example-asyncify")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --use-port=contrib.glfw3 -sASYNCIFY --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell.html")
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