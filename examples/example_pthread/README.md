### What does it do? 

The purpose of this example is to demonstrate how to use pthread which allows the code to run multiple threads concurrently

### Building instructions

Using `emcc` from the command line (make sure it is in your path)

```sh
mkdir build
emcc -pthread --use-port=contrib.glfw3 -sEXIT_RUNTIME=1 -sPROXY_TO_PTHREAD --shell-file=shell.html main.cpp -o build/index.html
```

Using `CMake`:

```cmake
set(target "example-pthread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread --use-port=contrib.glfw3 -sEXIT_RUNTIME=1 -sPROXY_TO_PTHREAD --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell.html")
add_executable(${target} main.cpp)
target_compile_options(${target} PUBLIC "-pthread")
set_target_properties(${target} PROPERTIES OUTPUT_NAME "index")
set_target_properties(${target} PROPERTIES SUFFIX ".html")
```

### Running instructions

> [!NOTE]
> Due to the [limitations](https://emscripten.org/docs/porting/pthreads.html) with browsers, this test needs to be run
> with `emrun` which automatically takes care of the right headers.


```sh
emrun --browser firefox build/main.html
```