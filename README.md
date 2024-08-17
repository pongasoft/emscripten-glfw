Introduction
------------

This project is an emscripten port of GLFW written in C++ for the web/wasm platform. The currently supported
GLFW API is 3.4.

[![emscripten - TBD](https://img.shields.io/badge/emscripten-TBD-blue)](https://emscripten.org)
[![Latest - 3.4.0.20240817](https://img.shields.io/badge/Latest-3.4.0.20240817-blue)](https://github.com/pongasoft/emscripten-glfw/releases/latest)
[![GLFW - 3.4.0](https://img.shields.io/badge/GLFW-3.4.0-blue)](https://www.glfw.org/)
![Compiles](https://github.com/pongasoft/emscripten-glfw/actions/workflows/main.yml/badge.svg)

[![emscripten - 3.1.63](https://img.shields.io/badge/emscripten-3.1.63-blue)](https://emscripten.org)
[![Version - 3.4.0.20240627](https://img.shields.io/badge/Version-3.4.0.20240627-blue)](https://github.com/pongasoft/emscripten-glfw/releases/latest)
[![GLFW - 3.4.0](https://img.shields.io/badge/GLFW-3.4.0-blue)](https://www.glfw.org/)

[![License](https://img.shields.io/badge/License-Apache%20License%202.0-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Goal
----

The main goal of this project is to implement as much as the GLFW API possible (in a browser context).

Since this project is targeting the web/webassembly platform, which runs in more recent web browsers, it is also trying
to focus on using the most recent features and not use deprecated features (for example, uses `keyboardEvent.key` 
vs `keyboardEvent.charcode`). As a result, this implementation will most likely not work in older browsers.

Since the code is written in C++, it is trying to minimize the amount of JavaScript code to remain clean and lean.

Features
--------

Main supported features:
* can create as many windows as you want, each one associated to a different canvas (use 
  `emscripten::glfw3::SetNextWindowCanvasSelector("#canvas2")` to specify which canvas to use)
* resizable window/canvas (use `emscripten::glfw3::MakeCanvasResizable(...)` to make the canvas resizable by user.
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
* clipboard (cut/copy/paste with external clipboard)
* timer

Demo
----

<a href="https://pongasoft.github.io/emscripten-glfw/test/demo/main.html">![emscripten_glfw](https://github.com/pongasoft/emscripten-glfw/releases/download/v3.4.0.20240727/emscripten-glfw.png)</a>

Check out the [live demo](https://pongasoft.github.io/emscripten-glfw/test/demo/main.html) of the example code. Note that you
need to use a "modern" browser to see it in action. Currently tested on Google Chrome 120+ and Firefox 121+. 

The [code](test/demo/src) for the demo is included in this project.

The demo shows 2 canvases each created via a `glfwCreateWindow` and shows how they respond to keyboard and mouse events
(using direct apis, like `glfwGetMouseButton` or callback apis like `glfwSetMouseButtonCallback`)

- canvas1 is hi dpi aware (`glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE)`)
- canvas2 is **not** hi dpi aware (but can be made so with the "Enable" Hi DPI Aware button)
- canvas2 is fully resizable (use the square handle to resize) (`emscripten::glfw3::MakeCanvasResizable(window2, "#canvas2-container", "#canvas2-handle")`)

You can enable/disable each window/canvas independently:

- When 2 (or more) canvases are present, the canvas that has focus can receive keyboard events. If no other element on 
  the page has focus, then the last canvas that had the focus will receive these events. Clicking with the left mouse 
  button on a canvas gives it focus.
- When there is only 1 canvas, the implementation tries to be smart about it and will route keyboard (and other relevant) 
  events to the single canvas if nothing else has focus (the 'Change focus/Text' field is used to test 
  this feature since clicking on the text field grabs the focus).

The demo uses webgl to render a triangle (the hellow world of gpu rendering...).

Live Applications
-----------------

<table>
<thead>
<tr><th>Application</th><th>Description</th></tr>
</thead>
  <tbody>
  <tr>
    <td><a href="https://pongasoft.github.io/webgpu-shader-toy/">WebGPU Shader Toy</a> (<a href="https://github.com/pongasoft/webgpu-shader-toy">src</a>)</td>
    <td>WebGPU Shader Toy is a free and open source tool for experimenting with WebGPU fragment shaders and the WebGPU Shader Language (WGSL)</td>
  </tr>
  </tbody>
</table>

Examples (part of this project)
-------------------------------

<table>
<thead>
<tr><th>Example</th><th>Note</th></tr>
</thead>
  <tbody>
  <tr><td><a href="https://pongasoft.github.io/emscripten-glfw/test/demo/main.html">Demo</a> (<a href="test/demo">src</a>)</td><td>Main test/demo which demonstrates most features of the implementation</td></tr>
  <tr>
    <td><a href="https://pongasoft.github.io/emscripten-glfw/examples/example_asyncify/main.html">example_asyncify</a> (<a href="examples/example_asyncify">src</a>)</td>
    <td>The purpose of this example is to demonstrate how to use asyncify which allows the code to be written like you
      would for a normal desktop application</td>
  </tr>
  <tr>
    <td><a href="https://pongasoft.github.io/emscripten-glfw/examples/example_hi_dpi/main.html">example_hi_dpi</a> (<a href="examples/example_hi_dpi">src</a>)</td>
    <td>The purpose of this example is to demonstrate how to make the window Hi DPI aware</td>
  </tr>
  <tr>
    <td><a href="https://pongasoft.github.io/emscripten-glfw/examples/example_minimal/main.html">example_minimal</a> (<a href="examples/example_minimal">src</a>)</td>
    <td>The purpose of this example is to be as minimal as possible: initializes glfw, creates a window, then destroys it and terminates glfw.
      Uses the default shell that comes with emscripten</td>
  </tr>
  <tr>
    <td><a href="https://pongasoft.github.io/emscripten-glfw/examples/example_resizable_container/main.html">example_resizable_container</a> (<a href="examples/example_resizable_container">src</a>)</td>
    <td>The purpose of this example is to demonstrate how to make the canvas resizable with another container (a
      surrounding div) driving its size. The container width is proportional to the size of the window and so as the
      window gets resized so does the div, and so does the canvas</td>
  </tr>
  <tr>
    <td><a href="https://pongasoft.github.io/emscripten-glfw/examples/example_resizable_container_with_handle/main.html">example_resizable_container_with_handle</a> (<a href="examples/example_resizable_container_with_handle">src</a>)</td>
    <td>The purpose of this example is to demonstrate how to make the canvas resizable with a container that has a handle.
      The handle can be dragged around (left mouse drag), and the div is resized accordingly which in turn resizes the
      canvas, making the canvas truly resizable like a window</td>
  </tr>
  <tr>
    <td><a href="https://pongasoft.github.io/emscripten-glfw/examples/example_resizable_full_window/main.html">example_resizable_full_window</a> (<a href="examples/example_resizable_full_window">src</a>)</td>
    <td>The purpose of this example is to demonstrate how to make the canvas resizable and occupy the full window</td>
  </tr>
  </tbody>
</table>

Integration
-----------

Since ImGui [v1.91.0](https://github.com/ocornut/imgui/releases/tag/v1.91.0),
ImGui can be configured to use this port, allowing full gamepad and clipboard support amongst many other advantages. 

Usage
-----

Check the [Usage](docs/Usage.md) documentation for details on how to use this implementation. Note that care has been
taken to be backward compatible with the pure JavaScript implementation built-in in emscripten.

Building
--------

### Using emscripten port

Since emscripten 3.1.55, using this port is really easy via the `--use-port=contrib.glfw3` option 
(no need to clone this repo at all!). This is the recommended method to use this project.

Example:

```sh
emcc --use-port=contrib.glfw3 main.cpp -o build/index.html
```

The port can be configured with the following options:

| Option               | Description                                                                                               |
|----------------------|-----------------------------------------------------------------------------------------------------------|
| `disableJoystick`    | Boolean to disable support for joystick entirely, which can be useful if you don't need it due to polling |
| `disableWarning`     | Boolean to disable warnings emitted by the library (for example when using non supported features)        |
| `disableMultiWindow` | Boolean to disable multi window support which makes the code smaller and faster if you don't need it      |

Example using `disableWarning` and `disableMultiWindow`:

```sh
emcc --use-port=contrib.glfw3:disableWarning=true:disableMultiWindow=true main.cpp -o build
```

> #### Note about availability in emscripten
> | emscripten | this port      |
> |------------|----------------|
> | TBD        | 3.4.0.20240817 |
> | TBD        | 3.4.0.20240804 |
> | TBD        | 3.4.0.20240731 |
> | TBD        | 3.4.0.20240727 |
> | 3.1.63     | 3.4.0.20240627 |
> | 3.1.60     | 3.4.0.20240514 |
> | 3.1.57     | 3.4.0.20240318 |
> | 3.1.56     | 1.1.0          |
> | 3.1.55     | 1.0.5          |
> 
> Due to the release cadence of emscripten, if you want to be in charge of which version you use, you can simply
> use the port that is checked-in under `port`: `--use-port=port/emscripten-glfw3.py`


> #### Note
> Emscripten automatically downloads and builds the library for you and stores it in its cache. If you want to
> delete the library from the cache, you can use the `embuilder` tool:
> ```sh
> # remove library from cache
> embuilder clear contrib.glfw3
> # remove library from cache (with options)
> embuilder clear contrib.glfw3:disableWarning=true:disableMultiWindow=true
> ```

### CMake

If you use CMake, you should be able to simply add this project as a subdirectory. Check
[CMakeLists.txt](test/demo/CMakeLists.txt) for an example of the build options used.

With CMake, you can set the (CMake) option `EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK` if your application does not care about
supporting joystick as it can be an extra burden on size and runtime polling.

You can also set the (CMake) option `EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT` if your application does not need
multi window support, and you want a smaller code and faster execution.

When compiling in `Release` mode, the compilation flag `EMSCRIPTEN_GLFW3_DISABLE_WARNING` is automatically set.

### Makefile

For testing purposes, I am successfully building ImGui (`examples/example_emscripten_wgpu`) against this 
implementation with the following section in the `Makefile`:

```Makefile
# local glf3 port
EMS_GLFW3_DIR = /Volumes/Development/github/org.pongasoft/emscripten-glfw
SOURCES += $(EMS_GLFW3_DIR)/src/cpp/glfw3.cpp
SOURCES += $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Clipboard.cpp \
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Context.cpp \
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/ErrorHandler.cpp \
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Keyboard.cpp \
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Joystick.cpp \
           $(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/Window.cpp

CPPFLAGS += -I$(EMS_GLFW3_DIR)/include -I$(EMS_GLFW3_DIR)/external -DEMSCRIPTEN_USE_PORT_CONTRIB_GLFW3

# ("EMS" options gets added to both CPPFLAGS and LDFLAGS, whereas some options are for linker only)
#EMS += -s DISABLE_EXCEPTION_CATCHING=1
#LDFLAGS += -s USE_GLFW=3 -s USE_WEBGPU=1
LDFLAGS += -s USE_WEBGPU=1 --js-library $(EMS_GLFW3_DIR)/src/js/lib_emscripten_glfw3.js
#LDFLAGS += -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1

%.o:$(EMS_GLFW3_DIR)/src/cpp/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o:$(EMS_GLFW3_DIR)/src/cpp/emscripten/glfw3/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
```

> ### Note
> The previous `Makefile` is shown as an illustration of what a `Makefile` to compile this project 
> would look like. The actual changes required for ImGui are actually much simpler:
> ```Makefile
> EMS += -s DISABLE_EXCEPTION_CATCHING=1 --use-port=contrib.glfw3
> #LDFLAGS += -s USE_GLFW=3 -s USE_WEBGPU=1
> LDFLAGS += -s USE_WEBGPU=1
> ```

Release Notes
-------------
#### 3.4.0.20240817 - 2024-08-17 | emscripten TBD

- Major clipboard changes: the clipboard now uses the browser events to handle cut, copy and paste
- Added a way to [tweak the timeouts](docs/Usage.md#keyboard-support) for the Super + Key workaround (Super is also known as Meta or Cmd)
- Added a way to set which keys are allowed to be [handled by the browser](docs/Usage.md#keyboard-support)
- Added a convenient API to open a URL (`emscripten::glfw3::OpenURL`)
- Added a convenient API to detect if the runtime platform is Apple (`emscripten::glfw3::IsRuntimePlatformApple`),
  mostly used for keyboard shortcuts (Ctrl vs. Cmd). 
- Added `GLFW/emscripten_glfw3_version.h` with `EMSCRIPTEN_GLFW_VERSION` define for compilation time version detection
- `EMSCRIPTEN_USE_PORT_CONTRIB_GLFW3` port define now also contains the version
 
> [!WARNING]
> Breaking changes!
> The clipboard async API has been removed.
> Check the [Clipboard support](docs/Usage.md#clipboard-support) section for details on how to deal with the 
> clipboard in your application.

#### 3.4.0.20240804 - 2024-08-04 | emscripten TBD

- Fixed `nullptr` issue when clipboard is empty
- Fixed the internal clipboard being wiped on asynchronous callback error

#### 3.4.0.20240731 - 2024-07-31 | emscripten TBD

- Added `emscripten_glfw_get_clipboard_string` the C version of  `emscripten::glfw3::GetClipboardString` to
  retrieve the clipboard asynchronously
- Added a helper class `emscripten::glfw3::FutureClipboardString` to greatly simplify the more frequent use-cases
- `GetClipboardString::value()` now returns the internal clipboard in case of error, instead of throwing exception
- Added `optimizationLevel` option to the emscripten port

#### 3.4.0.20240727 - 2024-07-27 | emscripten TBD

- Introduced C++ API (namespace `emscripten::glfw3`) included with `GLFW3/emscripten_glfw3.h`:
  - provides a more correct API with sensible defaults (ex: `std::string_view` / `std::optional<std::string_view>` 
    vs `char const *` which may or may not be `nullptr`)
  - allow for C++ only API (ex: `std::future`)
  - the C API is still available if you would rather stick to it
- Implemented  `emscripten::glfw3::GetClipboardString` which provides a way of fetching the global
  clipboard in a browser environment (`glfwGetClipboardString` is not the right API due to the asynchronous nature 
  of the underlying platform API).
- The cursor position is no longer clamped to the window size, and as a result, can have negative values or values
  greater than the window size.
  Note that GLFW implements a similar behavior on the macOS desktop platform.
- Implemented `glfwSetWindowPosCallback`
- Added support for GLFW Window Attribute `GLFW_HOVERED`
- Fixed [#6](https://github.com/pongasoft/emscripten-glfw/issues/6): _`emscripten_glfw_make_canvas_resizable` does not clean up properly_.
- Fixed an issue with opacity: when using opacity, the handle is not working unless its z-index is higher than the 
  canvas z-index

#### 3.4.0.20240627 - 2024-06-27 | emscripten 3.1.63

- Fixed internal implementation to use `EM_BOOL` (PR [#5](https://github.com/pongasoft/emscripten-glfw/pull/5))

#### 3.4.0.20240625 - 2024-06-25 | emscripten 3.1.63

- Implemented workaround for [#4](https://github.com/pongasoft/emscripten-glfw/issues/4): _Using Super + "Key" on macOS results in "Key" not being released_.
  Due to the [broken state](https://stackoverflow.com/questions/11818637/why-does-javascript-drop-keyup-events-when-the-metakey-is-pressed-on-mac-browser) of 
  JavaScript handling the `Super/Meta` key, there is no good solution. The workaround implemented, releases all keys when `Super` is released. Although not a perfect
  solution, it guarantees that the state is _eventually_ consistent:
    - if "Key" was released while "Super" was held, then when "Super" gets released, "Key" is released (later than when actually released, final state is consistent: "Key" in `Release` state)
    - if "Key" is still held when "Super" is released, "Key" is released when "Super" gets released, but immediately gets a down event (Up/Down event, final state is consistent": "Key" in `Pressed` state)

#### 3.4.0.20240617 - 2024-06-17 | emscripten 3.1.63

- Fixed [#3](https://github.com/pongasoft/emscripten-glfw/issues/3): _glfwGetKey must return one of `GLFW_PRESS` or `GLFW_RELEASE`_

#### 3.4.0.20240616 - 2024-06-16 | emscripten 3.1.63

- Implemented `glfwGetClipboardString`. Note that due to the async (and restrictive) nature of the 
  `navigator.clipboard.readText` call, this synchronous API returns whatever was set via a previous call
  to `glfwSetClipboardString` and ignores the external clipboard entirely.

#### 3.4.0.20240601 - 2024-06-01 | emscripten 3.1.63

- Fixed [#2](https://github.com/pongasoft/emscripten-glfw/issues/2): Dynamically changing HiDPI awareness does not trigger content callback

#### 3.4.0.20240514 - 2024-05-14 | emscripten 3.1.60

- Implemented `glfwSetClipboardString`

#### 3.4.0.20240501 - 2024-05-01 | emscripten 3.1.60

- Fixed issue when calling `glfwGetWindowContentScale` with `nullptr`
- Renamed javascript api/impl since these names are not unique to avoid potential conflicts
- Added `glfw3native.h` to `GLFW`. Although not used (at this moment) by this implementation, this allows
  calling code to include it if necessary since it is part of a normal GLFW installation.

#### 3.4.0.20240318 - 2024-03-18 | emscripten 3.1.57

- Fixed joystick/gamepad code that was improperly mapping `GLFW_GAMEPAD_AXIS_LEFT_TRIGGER`
  `GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER`, `GLFW_GAMEPAD_BUTTON_LEFT_THUMB` and `GLFW_GAMEPAD_BUTTON_RIGHT_THUMB`
- `GLFW_GAMEPAD_AXIS_LEFT_TRIGGER` and `GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER` are now properly represented as an analog
  value in the range [-1.0, +1.0]
- Please note the change in version numbering, which from now on will be tied to the GLFW version implemented + date
  of release of this port to avoid confusion

#### 1.1.0 - 2024-02-29 | emscripten 3.1.56

- Upgraded to GLFW 3.4
- GLFW 3.4 features implemented
  - `glfwGetPlatform` and `glfwPlatformSupported` uses the `GLFW_PLATFORM_EMSCRIPTEN` constant defined in 
    `emscripten-glfw3.h` (officially reserved value is `0x00060006`)
  - `glfwInitHint` supports the new hint `GLFW_PLATFORM`
  - Supports all 10 cursors
  - Implemented `glfwGetWindowTitle`
  - Use `GLFW_SCALE_FRAMEBUFFER` to enable (resp. disable) Hi DPI support
  - Changed the functions that can report `GLFW_FEATURE_UNAVAILABLE` failure to report this error instead of a warning
- GLFW 3.4 features not implemented
  - `GLFW_MOUSE_PASSTHROUGH` is not supported
  - `GLFW_CURSOR_CAPTURED` cursor input mode is not supported (not possible in a browser context)
  - `glfwInitAllocator` is implemented as noop (could be supported for the C++ part only if there is demand, not javascript)
  - `GLFW_POSITION_X` and `GLFW_POSITION_Y` are not supported (same as `glfwSetWindowPos`)
  - `GLFW_ANGLE_PLATFORM_TYPE` is not supported (no direct access in browser, but implementation is most likely using it anyway) 
- Since GLFW 3.4 introduces a proper constant to handle Hi DPI Awareness (`GLFW_SCALE_FRAMEBUFFER`), the prior
  constant used by this port (`GLFW_SCALE_TO_MONITOR`) is still accepted, but it is now deprecated. In addition,
  due to the fact that `GLFW_SCALE_FRAMEBUFFER` defaults to `GLFW_TRUE`, this port is now Hi DPI aware by default and
  needs to be explicitly turned off (`glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE)`) if this is the desired
  behavior.

#### 1.0.5 - 2024/02/18 | emscripten 3.1.55

- Fixed memory corruption with joystick

#### 1.0.4 - 2024/01/24

- Fixed version string

#### 1.0.3 - 2024/01/23

- Fixed `emscripten_glfw3.h` to work as a C file
- Added `create-archive` target

#### 1.0.2 - 2024/01/22

- Added `EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK` as a CMake option
- Made joystick code truly conditional on `EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK` define
- Added `EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT` as a CMake option and made the multi window code conditional
  on `EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT` define
- Misc: added GitHub workflow to compile the code/display badge

#### 1.0.1 - 2024/01/21
 
- Fixed import

#### 1.0.0 - 2024/01/21

- First 1.0.0 release
- Added examples
- Added documentation
- Fixed some issues
- Removed `GLFW_EMSCRIPTEN_CANVAS_SELECTOR` window hint in favor of a new api `emscripten_glfw_set_next_window_canvas_selector`
- Removed `GLFW_EMSCRIPTEN_CANVAS_RESIZE_SELECTOR` and `Module.glfwSetCanvasResizableSelector` in favor of a new 
  api `emscripten_glfw_make_canvas_resizable`
- This new api also offer the ability to deal with the handle automatically
- Implemented `getWindowPosition` (canvas position in the browser window)
- Implemented all timer apis (`glfwSetTime`, `glfwGetTimerValue` and `glfwGetTimerFrequency`)
- Implemented `glfwExtensionSupported`
- Implemented `glfwSetWindowTitle` (changes the browser window title)

#### wip-0.5.0 - 2024/01/12

- Added support for resizable canvas (`glfwWindowHintString(GLFW_EMSCRIPTEN_CANVAS_RESIZE_SELECTOR, "#canvas2-container")` 
  from c/c++ code or `Module.glfwSetCanvasResizableSelector('#canvas2', '#canvas2-container')` from javascript) 
- Added support fo visibility (`glfwShowWindow` and `glfwHideWindow`)
- Added support for `GLFW_FOCUS_ON_SHOW` window hint/attribute
- Added support for dynamic Hi DPI Awareness (`GLFW_SCALE_FRAMEBUFFER` can be used in `glfwSetWindowAttrib`)
- Added support for "sticky" mouse button and keyboard
- Added support for window size constraints (`glfwSetWindowSizeLimits` and `glfwSetWindowAspectRatio`)
- Added support for providing a callback function in javascript to be notified when a window is created (`Module.glfwOnWindowCreated`)

#### wip-0.4.0 - 2024/01/03

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
