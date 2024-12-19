## Introduction

This section describes how to build this project.

> [!NOTE]
> If you want to **use** this project, the recommended approach is to use the [Emscripten port](../README.md#quick-start-emscripten-port).
> This section is only meant if you want to **work** on this project.

## Building

The easiest way to build this project and run the examples, is to use `emcmake` and `emrun`.

Assuming Emscripten is in your path:

For `Debug` configuration:

```sh
emcmake cmake -B build_debug -DCMAKE_BUILD_TYPE=Debug .
cd build_debug
cmake --build . --target examples-all
emrun --browser firefox index.html 
```

For `Release` configuration:

```sh
emcmake cmake -B build_release -DCMAKE_BUILD_TYPE=Release .
cd build_release
cmake --build . --target examples-all
emrun --browser firefox index.html 
```