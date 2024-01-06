Introduction
------------

This project is an emscripten port of glfw written in C++ for the web/wasm platform.

Goal
----

The main goal of this project is to provide as many features from glfw as possible (in a browser context).

Since this project is targeting the web/wasm platform, which runs in more recent web browsers, it is also trying
to focus on using the most recent features and not use deprecated features (for example, uses `keyboardEvent.key` 
vs `keyboardEvent.charcode`). As a result, this implementation will most likely not work in older browsers.

Since the code is written in C++, it is trying to minimize the amount of javascript code to remain clean and lean.

A wishful goal would be to potentially replace the built-in emscripten/glfw code in the future if this effort 
is successful, or most likely, offer it as an option.

Status
------

This project is currently a work in progress, but I decided to release it early in case there is interest at this moment.

Main supported features:
* can create as many windows as you want, each one associated to a different canvas (use 
  `glfwWindowHintString(GLFW_EMSCRIPTEN_CANVAS_SELECTOR, "#canvas2")` to specify which canvas to use)
* mouse support
* keyboard support
* joystick/gamepad support
* fullscreen support
* Hi DPI support 
* support all glfw cursors
* support window opacity

You can check [glfw3.cpp](src/cpp/glfw3.cpp) for what is currently not implemented (throw `not_implemented` exception) 
or functions that have an empty (implementation `TODO Implement` section).

Demo
----

![emscripten_glfw](https://github.com/pongasoft/emscripten-glfw/releases/download/wip-0.4.0/emscripten_glfw.png)

Checkout the [live demo](https://pongasoft.github.io/emscripten-glfw/demo/main.html) of the example code. Note that you
need to use a "modern" browser to see it in action. Currently tested on Google Chrome 120+ and Firefox 121+. 

The [code](test/client/src) for the demo is included in this project.

The demo shows 2 canvases each created via a `glfwCreateWindow` and shows how they respond to keyboard and mouse events
(using direct apis, like `glfwGetMouseButton` or callback apis like `glfwSetMouseButtonCallback`)

- canvas1 is hi dpi aware (`glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE)`)
- canvas2 is **not** hi dpi aware

You can enable/disable each window/canvas independently:

- When 2 (or more) canvases are present, the canvas that has focus can receive keyboard events. If no other element on 
  the page has focus, then the last canvas that had the focus will receive these events. Clicking with the left mouse 
  button on a canvas gives it focus.
- When there is only 1 canvas, the implementation try to be smart about it and will route keyboard (and other relevant) 
  events to the single canvas provided that nothing else has focus (the 'Change focus/Text' field is used to test 
  this feature since clicking in the text field grabs the focus).

The demo uses webgl to render a triangle (the hellow world of gpu rendering...).

Using
-----

Because it is currently a work in progress, the instructions to use will be minimal.

In order to support multiple windows/canvases, the library need to know which canvas to use for which window. I decided
to go with using a new window hint. Note that the constant I am using is not officially added to glfw3, but I am hoping
it can be added in a future version when the project matures.

```cpp
#define GLFW_EMSCRIPTEN_CANVAS_SELECTOR  0x00027001 // hopefully will be part of glfw3.h someday

    glfwWindowHintString(GLFW_EMSCRIPTEN_CANVAS_SELECTOR, "#canvas1");
    auto window1 = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
```

To be backward compatible with the current emscripten/glfw/javascript implementation, the default canvas selector is 
set to `Module['canvas']` so you don't need to provide one.

To trigger fullscreen, you use `Module.requestFullscreen` like in the emscripten/glfw/javascript implementation, with
an optional 3rd parameter:
* If you provide a third parameter, it should be the canvas selector describing which canvas to use
* If you don't provide a 3rd parameter, the library does its best to determine which canvas to select:
  * if only 1 canvas, then it is used
  * if 2 (or more) canvases, then the last one that had the focus is used

Building
--------

Because it is currently a work in progress, the instructions to build will be minimal. 

### CMake

If you use CMake, you should be able to simply add this project as a subdirectory. Check 
[CMakeLists.txt](test/client/CMakeLists.txt) for an example of the build options used. 

### Makefile

I am successfully building ImGui (`examples/example_emscripten_wgpu`) against this implementation with the following section in the `Makefile`:

```Makefile
# local glf3 port
EMS_GLFW3_DIR = /Volumes/Development/github/org.pongasoft/emscripten-glfw
SOURCES += $(EMS_GLFW3_DIR)/src/cpp/glfw3.cpp
SOURCES += $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Context.cpp \
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/ErrorHandler.cpp \
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Keyboard.cpp \
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Window.cpp

# ("EMS" options gets added to both CPPFLAGS and LDFLAGS, whereas some options are for linker only)
#EMS += -s DISABLE_EXCEPTION_CATCHING=1
#LDFLAGS += -s USE_GLFW=3 -s USE_WEBGPU=1
LDFLAGS += -s USE_WEBGPU=1 --js-library $(EMS_GLFW3_DIR)/src/js/lib_emscripten_glfw3.js
#LDFLAGS += -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1
```

Release Notes
-------------

#### wip-0.4.0 - 2023/01/03

- Added support for joystick/gamepad
  - Joystick support can be disabled via `EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK` compilation flag

#### wip-0.3.0 - 2023/12/31

- Added support for input mode `GLFW_CURSOR` (handle all use cases: Normal / Hidden / Locked)
- Added support for glfw defined cursors (implemented `glfwCreateStandardCursor` and `glfwSetCursor`)
- Added support for window opacity (implemented `glfwGetWindowOpacity` and `glfwSetWindowOpacity`)

#### wip-0.2.0 - 2023/12/28

- remembers the last window that had focus so that some events can be sent to it even if no window has 
  focus (ex: requesting fullscreen)
- added support for mouse wheel (`glfwSetScrollCallback`)
- added support for mouse enter/leave (`glfwSetCursorEnterCallback`)

#### wip-0.1.0 - 2023/12/26

- first public version


Misc
----

This project includes the `glfw3.h` header (`external/GLFW/glfw3.h`) which uses a [ZLib license](https://www.glfw.org/license.html)

Licensing
---------

- Apache 2.0 License. This project can be used according to the terms of the Apache 2.0 license.
