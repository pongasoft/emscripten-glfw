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
  `emscripten_glfw_set_next_window_canvas_selector("#canvas2")` to specify which canvas to use)
* resizable window/canvas (use `emscripten_glfw_make_canvas_resizable(...)` to make the canvas resizable by user.
  Use `"window"` as the resize selector for full frame canvas (ex: ImGui))
* mouse (includes sticky button behavior)
* keyboard (includes sticky key behavior)
* joystick/gamepad
* fullscreen
* Hi DPI 
* all glfw cursors
* window opacity
* size constraints (size limits and aspect ratio)
* visibility
* focus
* timer

You can check [glfw3.cpp](src/cpp/glfw3.cpp) for what is currently not implemented. APIs that do not have an implementation for this
platform will display a warning (can be turned off with `EMSCRIPTEN_GLFW3_DISABLE_WARNING` define).

Demo
----

![emscripten_glfw](https://github.com/pongasoft/emscripten-glfw/releases/download/wip-0.5.0/emscripten_glfw.png)

Checkout the [live demo](https://pongasoft.github.io/emscripten-glfw/demo/main.html) of the example code. Note that you
need to use a "modern" browser to see it in action. Currently tested on Google Chrome 120+ and Firefox 121+. 

The [code](test/client/src) for the demo is included in this project.

The demo shows 2 canvases each created via a `glfwCreateWindow` and shows how they respond to keyboard and mouse events
(using direct apis, like `glfwGetMouseButton` or callback apis like `glfwSetMouseButtonCallback`)

- canvas1 is hi dpi aware (`glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE)`)
- canvas2 is **not** hi dpi aware (but can be made so with the "Enable" Hi DPI Aware button)
- canvas2 is fully resizable (use the square handle to resize) (`emscripten_glfw_make_canvas_resizable(window2, "#canvas2-container", "#canvas2-handle")`)

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

In order to support multiple windows/canvases, the library need to know which canvas to use for which window. You use
the function `emscripten_glfw_set_next_window_canvas_selector` to specify this association.

```cpp
#include <GLFW/emscripten_glfw3.h> // contains the definitions

emscripten_glfw_set_next_window_canvas_selector("#canvas1");
auto window1 = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
```

To be backward compatible with the current emscripten/glfw/javascript implementation, the default canvas selector is 
set to `Module['canvas']` so you don't need to call this function if you use this method.

To trigger fullscreen, you use `Module.glfwRequestFullscreen(target, lockPointer, resizeCanvas)` with
* `target` being which canvas need to be fullscreen
* `lockPointer`: boolean to enable/disable grabbing the mouse pointer (equivalent to calling `glfwSetInputMode(GLFW_CURSOR, xxx)`)
* `resizeCanvas`: boolean to resize (or not) the canvas to the fullscreen size

To be backward compatible with the current emscripten/glfw/javascript implementation, you can also call 
`Module.requestFullscreen(lockPointer, resizeCanvas)` and the library does its best to determine which
canvas to target.

### `emscripten_glfw_make_canvas_resizable`

If you want the canvas (= window) size to be adjusted dynamically by the user you can call the
convenient function `emscripten_glfw_make_canvas_resizable`. Although you can implement this functionality yourself, 
the implementation can be tricky to get right.

Since this library takes charge of the size of the canvas, the idea behind this function is to specify which
other (html) element can dictate the size of the canvas. The parameter `canvasResizeSelector` defines the
(css path) selector to this element.

The 3 typical uses cases are:

#### 1. Full window
The canvas fills the entire browser window, in which case the parameter `canvasResizeSelector` should simply
be set to "window" and the `handleSelector` is `nullptr`. This use case can be found in application like ImGui
where the canvas is the window.
    
Example code:
```html
<!-- html -->
<canvas id="canvas1"></canvas>
```
```cpp
// cpp
emscripten_glfw_set_next_window_canvas_selector("#canvas1");
auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
emscripten_glfw_make_canvas_resizable(window, "window", nullptr);
```

#### 2. Container (`div`)
The canvas is inside a `div`, in which case the `div` acts as a "container" and the `div` size is defined by
CSS rules, like for example: `width: 85%` so that when the page/browser gets resized, the `div` is resized
automatically, which then triggers the canvas to be resized. In this case, the parameter `canvasResizeSelector`
is the (css path) selector to this `div` and `handleSelector` is `nullptr`.

Example code:
```html
<!-- html -->
<style>#canvas1-container { width: 85%; height: 85% }</style>
<div id="canvas1-container"><canvas id="canvas1"></canvas></div>
```

```cpp
// cpp
emscripten_glfw_set_next_window_canvas_selector("#canvas1");
auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
emscripten_glfw_make_canvas_resizable(window, "#canvas1-container", nullptr);
```

#### 3. Container + handle
Same as 2. but the `div` is made resizable dynamically via a little "handle" (which ends up behaving like a
normal desktop window).

Example code:

```html
<!-- html -->
<style>#canvas1-container { position: relative; <!-- ... --> }</style>
<style>#canvas1-handle { position: absolute; bottom: 0; right: 0; background-color: #444444; width: 10px; height: 10px; cursor: nwse-resize; }</style>
<div id="canvas1-container"><div id="canvas1-handle" class="handle"></div><canvas id="canvas1"></canvas></div>
```

```cpp
// cpp
emscripten_glfw_set_next_window_canvas_selector("#canvas1");
auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
emscripten_glfw_make_canvas_resizable(window, "#canvas1-container", "canvas1-handle");
```

Check the [live demo](https://pongasoft.github.io/emscripten-glfw/demo/main.html) for a complete example.

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
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Joystick.cpp \
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Window.cpp

# ("EMS" options gets added to both CPPFLAGS and LDFLAGS, whereas some options are for linker only)
#EMS += -s DISABLE_EXCEPTION_CATCHING=1
#LDFLAGS += -s USE_GLFW=3 -s USE_WEBGPU=1
LDFLAGS += -s USE_WEBGPU=1 --js-library $(EMS_GLFW3_DIR)/src/js/lib_emscripten_glfw3.js
#LDFLAGS += -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1
```

Release Notes
-------------

#### wip-0.6.0 - TBD

- Removed `GLFW_EMSCRIPTEN_CANVAS_SELECTOR` window hint in favor of a new api `emscripten_glfw_set_next_window_canvas_selector`
- Removed `GLFW_EMSCRIPTEN_CANVAS_RESIZE_SELECTOR` and `Module.glfwSetCanvasResizableSelector` in favor of a new 
  api `emscripten_glfw_make_canvas_resizable`
- This new api also offer the ability to deal with the handle automatically
- Implemented `getWindowPosition` (canvas position in the browser window)
- Implemented all timer apis (`glfwSetTime`, `glfwGetTimerValue` and `glfwGetTimerFrequency`)
- Implemented `glfwExtensionSupported`
- Implemented `glfwSetWindowTitle` (changes the browser window title)

#### wip-0.5.0 - 2023/01/12

- Added support for resizable canvas (`glfwWindowHintString(GLFW_EMSCRIPTEN_CANVAS_RESIZE_SELECTOR, "#canvas2-container")` 
  from c/c++ code or `Module.glfwSetCanvasResizableSelector('#canvas2', '#canvas2-container')` from javascript) 
- Added support fo visibility (`glfwShowWindow` and `glfwHideWindow`)
- Added support for `GLFW_FOCUS_ON_SHOW` window hint/attribute
- Added support for dynamic Hi DPI Awareness (`GLFW_SCALE_TO_MONITOR` can be used in `glfwSetWindowAttrib`)
- Added support for "sticky" mouse button and keyboard
- Added support for window size constraints (`glfwSetWindowSizeLimits` and `glfwSetWindowAspectRatio`)
- Added support for providing a callback function in javascript to be notified when a window is created (`Module.glfwOnWindowCreated`)

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
