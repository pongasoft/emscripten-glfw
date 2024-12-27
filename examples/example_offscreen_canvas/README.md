### What does it do? 

The purpose of this example is to demonstrate the use of OffscreenCanvas which enables using WebGL contexts in pthreads

### Building instructions

Using `emcc` from the command line (make sure it is in your path)

```sh
mkdir build
emcc --use-port=contrib.glfw3 -pthread -sPROXY_TO_PTHREAD -sOFFSCREENCANVAS_SUPPORT -sOFFSCREENCANVASES_TO_PTHREAD='canvas' -sMIN_WEBGL_VERSION=2 --shell-file=shell.html main.cpp -o build/index.html
```

```cmake
set(target "example-offscreen-canvas")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread --use-port=contrib.glfw3 -sEXIT_RUNTIME=1 -sOFFSCREENCANVAS_SUPPORT -sOFFSCREENCANVASES_TO_PTHREAD='canvas' -sPROXY_TO_PTHREAD -s SAFE_HEAP=1 -s ASSERTIONS=1 -s WASM=1 --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell.html")
add_executable(${target} main.cpp)
target_compile_options(${target} PUBLIC "-pthread")
set_target_properties(${target} PROPERTIES OUTPUT_NAME "main")
set_target_properties(${target} PROPERTIES SUFFIX ".html")
target_link_libraries(${target} PRIVATE glfw3_pthread)
```

### Running instructions

> [!NOTE]
> Due to the [limitations](https://emscripten.org/docs/porting/pthreads.html) with browsers, this test needs to be run
> with `emrun` which automatically takes care of the right headers.

```sh
emrun --browser firefox build/main.html
```