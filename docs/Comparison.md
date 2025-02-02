## Introduction

Emscripten comes with a built-in implementation of GLFW (3) written in 100% JavaScript.
This implementation is an alternate and more complete implementation of the GLFW API (currently 3.4.0).
This document explains the differences.


## Brief History

From the git history and the current source code, it is possible to gather the facts that the built-in GLFW 
implementation was implemented back in 2013 in pure JavaScript.
It currently states that it implements the `3.2.1` version of the API released in 2016 
(as returned by `glfwGetVersion`).
There have been 2 major releases since then (`3.3.0` released in 2019 and `3.4.0` released in 2024).
It does not comply with the error handling paradigm of GLFW (`glfwSetErrorCallback` stores the function pointer but 
never uses it and some APIs throw an exception when called (like `glfwGetTimerFrequency`)).

This library currently implements the latest GLFW API (3.4.0) in C++ (and JavaScript when necessary), 
using more modern browser technologies and addresses some issues from the JavaScript implementation.
It tries to do its best to implement as much of the GLFW API that is possible to implement in the context 
of a web browser. When it can't, it handles the case gracefully and in compliance with error handling in GLFW.

> [!NOTE]
> This implementation has taken great care to be backward compatible with the built-in implementation,
> for example, supporting `Module['canvas']`, so that it is a drop-in replacement.


## How to use?

When compiling using the Emscripten compiler (`emcc` / `em++`), it is equally trivial to pick which flavor you want 
to use:

* If you want to use the built-in implementation, you use the syntax `-sUSE_GLFW=3`
* If you want to use this implementation, you use the syntax `--use-port=contrib.glfw3`

> [!TIP]
> When using `CMake`, you need to define it as both a compile and link option:
> ```cmake
> target_compile_options(${target} PUBLIC "--use-port=contrib.glfw3")
> target_link_options(${target} PUBLIC "--use-port=contrib.glfw3")
> ```

## Main features comparison

This section describes the main features implemented by both libraries.

<table>
<thead>
<tr>
  <th>Feature</th>
  <th><code>emscripten-glfw</code><br>(this implementation)</th>
  <th><code>library_glfw.js</code><br>(built-in implementation)</th>
</tr>
</thead>
  <tbody>
  <tr>
    <td>Single Window</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
  </tr>
  <tr>
    <td>Multiple Windows</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="No" src="https://img.shields.io/badge/No-aaaaaa"></td>
  </tr>
  <tr>
    <td>Mouse</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"> (includes sticky button behavior)</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"> (NO sticky button behavior)</td>
  </tr>
  <tr>
    <td>Touch (mobile)</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
  </tr>
  <tr>
    <td>Keyboard</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"> (includes sticky key behavior)</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"> (NO sticky key behavior)</td>
  </tr>
  <tr>
    <td>Keyboard / Meta Key</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"> (implements <a href="Usage.md#the-problem-of-the-super-key">workaround</a>)</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Broken-aa0000"></td>
  </tr>
  <tr>
    <td>Joystick</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
  </tr>
  <tr>
    <td>Gamepad</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="No" src="https://img.shields.io/badge/No-aaaaaa">  3.3.x feature</td>
  </tr>
  <tr>
    <td>Fullscreen</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
  </tr>
  <tr>
    <td>Hi DPI</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
  </tr>
  <tr>
    <td>Clipboard (copy/paste)</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="No" src="https://img.shields.io/badge/No-aaaaaa"></td>
  </tr>
  <tr>
    <td>Cursors (Standard + Custom)</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="No" src="https://img.shields.io/badge/No-aaaaaa"></td>
  </tr>
  <tr>
    <td>Window Size constraints (size limits and aspect ratio)</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="No" src="https://img.shields.io/badge/No-aaaaaa"></td>
  </tr>
  <tr>
    <td>Window Opacity</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="No" src="https://img.shields.io/badge/No-aaaaaa"></td>
  </tr>
  <tr>
    <td>Window Status (Focused / Hovered / Position)</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="No" src="https://img.shields.io/badge/No-aaaaaa"></td>
  </tr>
  <tr>
    <td>Timer</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Exception-aa0000"> (throws exception if called!)</td>
  </tr>
  <tr>
    <td>Error Handing</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Broken-aa0000"></td>
  </tr>
  <tr>
    <td>Pthread / Offscreen canvas support</td>
    <td><img alt="Yes" src="https://img.shields.io/badge/Yes-00aa00"></td>
    <td><img alt="No" src="https://img.shields.io/badge/No-aaaaaa"></td>
  </tr>
  </tbody>
</table>

Please refer to the [exhaustive list](Usage.md#glfw-functions) of functions for further details.

> [!NOTE]
> This table is built with data from Emscripten 3.1.65 (2024/08/22)

> [!WARNING]
> Emscripten embeds `glfw3.h` version 3.3.8 yet implements version 3.2.1.
