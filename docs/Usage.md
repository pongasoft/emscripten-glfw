## Introduction

This emscripten/webassembly port of GLFW tries to implement as much as possible of the API. See the list of [supported
functions](#supported-functions) with some notes for details. This page documents the most important aspects
of the library.

## Main concept

This port, as well as other library ports (like [SDL](https://github.com/libsdl-org/SDL/blob/main/docs/README-emscripten.md)), associates the concept of a "window" (in this instance 
a `GLFWwindow`) to an html "canvas". The framebuffer size of the window is the size of the canvas 
(`canvas.width` x `canvas.height`) and this is what you use for your viewport.
The size of the window is the css style size of the canvas (which in the case of Hi DPI is different). The opacity
is the css style `opacity`, etc...

> #### Important
> Once the canvas is associated to the window, the library takes control over it and sets various listeners and
> css styles on the canvas. In particular, the width and height is controlled by the library and as a result this
> implementation offers another mechanism for the user to be able to resize the canvas.

### How to associate the window to the canvas?

Natively, GLFW doesn't know anything about the concept of a canvas. So there needs to be a way to make this association.
This library offers 2 ways depending on your needs:

#### 1. Using javascript/Module

Every emscripten application needs to define a `Module` variable in javascript (see [example](https://github.com/emscripten-core/emscripten/blob/900aee0a2df98b28579d72b17f6fa73e48087e69/src/shell.html#L37)).
By convention in emscripten, the `Module["canvas"]` field represents the canvas associated to the window.
To be backward compatible with this option, this library supports it, and it is the default. Obviously, this can only
work if there is only one window, which is why there is another method.

#### 2. Using `emscripten::glfw3::SetNextWindowCanvasSelector`

This implementation offers an alternative way of specifying which canvas to associate to which window: the function
`emscripten::glfw3::SetNextWindowCanvasSelector` which must be called **prior** to calling `glfwCreateWindow`. The
single argument to the function is a css path selector to the canvas.

Example:

```cpp
#include <GLFW/emscripten_glfw3.h> // contains the definitions

emscripten::glfw3::SetNextWindowCanvasSelector("#canvas1");
auto window1 = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
```

This function is required if you use more than one window since the `Module` solution only supports 1 canvas.
It also offers the advantage of defining the association in C/C++ as opposed to html/javascript.

### How to make the canvas resizable by the user?

GLFW deals with windows. Windows, in the context of a desktop application, are usually resizable by the user (note that
the GLFW window hint/attribute `GLFW_RESIZABLE` lets you disable this feature). So how does this translate into the
html/canvas world?

To make the canvas resizable, and behave more like a "traditional" window, this implementation offers a
convenient API: `emscripten::glfw3::MakeCanvasResizable`:

```cpp
int MakeCanvasResizable(GLFWwindow *window,
                        std::string_view canvasResizeSelector,
                        std::optional<std::string_view> handleSelector = std::nullopt);
```

Since this library takes control of the canvas size, the idea behind this function is to specify which
other (html) element dictates the size of the canvas. The parameter `canvasResizeSelector` defines the
(css path) selector to this element.

The 3 typical use cases are:

#### 1. Full window

The canvas fills the entire browser window, in which case the parameter `canvasResizeSelector` should simply
be set to "window" and the `handleSelector` is `std::nullopt`.
This use case can be found in application like ImGui where the canvas is the window.

Example code:

```html
<!-- html -->
<canvas id="canvas1"></canvas>
```

```cpp
// cpp
emscripten::glfw3::SetNextWindowCanvasSelector("#canvas1");
auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
emscripten::glfw3::MakeCanvasResizable(window, "window");
```

#### 2. Container (`div`)

The canvas is inside a `div`, in which case the `div` acts as a "container" and the `div` size is defined by
CSS rules, like for example: `width: 75vw` so that when the page/browser gets resized, the `div` is resized
automatically, which then triggers the canvas to be resized. In this case, the parameter `canvasResizeSelector`
is the (css path) selector to this `div` and `handleSelector` is `std::nullopt`.

Example code:

```html
<!-- html -->
<style>
  #canvas1-container {
    width: 75vw;
    height: 50vh;
  }
</style>
<div id="canvas1-container">
  <canvas id="canvas1"></canvas>
</div>
```

```cpp
// cpp
emscripten::glfw3::SetNextWindowCanvasSelector("#canvas1");
auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
emscripten::glfw3::MakeCanvasResizable(window, "#canvas1-container");
```

#### 3. Container + handle

Same as 2. but the `div` is made resizable dynamically via a little "handle" (which ends up behaving like a
normal desktop window).

Example code:

```html
<!-- html -->
<style>
  #canvas1-container {
    position: relative;
  <!-- . . . -->
  }

  #canvas1-handle {
    position: absolute;
    bottom: 0;
    right: 0;
    margin-bottom: 1px;
    margin-right: 1px;
    border-left: 20px solid transparent;
    border-bottom: 20px solid rgba(102, 102, 102, 0.5);
    width: 0;
    height: 0;
    cursor: nwse-resize;
    z-index: 1;
  }
</style>
<div id="canvas1-container">
  <div id="canvas1-handle" class="handle"></div>
  <canvas id="canvas1"></canvas>
</div>
```

```cpp
// cpp
emscripten::glfw3::SetNextWindowCanvasSelector("#canvas1");
auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
emscripten::glfw3::MakeCanvasResizable(window, "#canvas1-container", "canvas1-handle");
```

> #### Note
> If you do not want the canvas to be resizable by the user, you can simply set its size during window creation
> (`glfwCreateWindow`) or with `glfwSetWindowSize` and don't do anything else.

## Fullscreen support

GLFW has a concept of a fullscreen window.
This is quite tricky for this implementation due to the restrictions imposed by browsers to go fullscreen.
Historically, emscripten has offered a way to do it from javascript by the means of a
function that gets added automatically to the `Module` called `requestFullscreen`.

This implementation adds another javascript function `Module.glfwRequestFullscreen(target, lockPointer, resizeCanvas)`
with

* `target` being which canvas need to be fullscreen
* `lockPointer`: boolean to enable/disable grabbing the mouse pointer (equivalent to
  calling `glfwSetInputMode(GLFW_CURSOR, xxx)`)
* `resizeCanvas`: boolean to resize (or not) the canvas to the fullscreen size

To be backward compatible with the current emscripten/glfw/javascript implementation, you can also call
`Module.requestFullscreen(lockPointer, resizeCanvas)` and the library does its best to determine which
canvas to target.

This implementation also offers a CPP and C version of this API:

```cpp
void emscripten::glfw3::RequestFullscreen(GLFWwindow *window, bool lockPointer, bool resizeCanvas);
void emscripten_glfw_request_fullscreen(GLFWwindow *window, bool lockPointer, bool resizeCanvas);
```

> #### Best practice
> To avoid any error while switching to fullscreen, you should always trigger this api from within a user event
> like a mouse click (callback set via `glfwSetMouseButtonCallback`)
> or a keyboard key press (callback set via `glfwSetKeyCallback`)

At this moment, this implementation does not support creating a window in fullscreen mode due to the same browser
restrictions mentioned previously. If you want to create a fullscreen window, create a window with a fixed size,
then from a user event call `Module.glfwRequestFullscreen`.

## Hi DPI support

This implementation supports Hi DPI awareness. What this means is that if the browser window is on a screen that is
Hi DPI/4k, then it will properly adjust the dimension of the canvas to match the scale of the screen. If the window gets
moved to a screen that is lower resolution, it will automatically change the scaling. You can set a callback to be
notified of the changes (`glfwSetWindowContentScaleCallback`) or call the direct API `glfwGetWindowContentScale`.

By default, this feature is enabled and can be turned off like this:

```cpp
// before creating a window (to turn Hi DPI Awareness OFF)
glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);
auto window = glfwCreateWindow(...);

// after window creation, it can be dynamically changed
glfwSetWindowAttrib(window, GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE); // for enabling Hi DPI awareness
glfwSetWindowAttrib(window, GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE); // for disabling Hi DPI awareness
```

> #### Note
> The constant `GLFW_SCALE_FRAMEBUFFER` was introduced in GLFW 3.4. The constant `GLFW_SCALE_TO_MONITOR` which was 
> used prior to GLFW 3.4, can still be used to trigger Hi DPI Awareness, but is less descriptive and as a result it 
> is deprecated, and it is preferable to use `GLFW_SCALE_FRAMEBUFFER`.


> #### Best practice
> Almost all GLFW apis deal with screen coordinates which are independent of scaling.
> The only one that doesn't is `glfwGetFramebufferSize`
> which returns the actual size of the surface which takes into account the scaling factor.
> As a result, for most low-level APIs (like OpenGL/webgl) you would use this call to set the viewport size.
>
> Here is an example:
> ```cpp
> int width = 0, height = 0;
> glfwGetFramebufferSize(fWindow, &width, &height);
> glViewport(0, 0, width, height);
> ```

## Keyboard support

This implementation supports the keyboard and uses the same mapping defined in emscripten for scancodes. You can check
[KeyboardMapping.h](../src/cpp/emscripten/glfw3/KeyboardMapping.h) for the full mapping. This implementation uses `KeyboardEvent.key` to compute an accurate
codepoint (provided to the `GLFWcharfun` callback) and not the deprecated  `KeyboardEvent.charcode` like other
implementations.

## Joystick/Gamepad support

This implementation uses the javascript `Gamepad` API as defined in the [specification](https://w3c.github.io/gamepad/)
which is widely supported by most current browsers.

> #### Important
> Due to the nature of the `Gamepad` API, polling is required, so you must ensure to call `glfwPollEvents` on each
> loop iteration.
>
> If you want to disable joystick support entirely (and save some resources), you can use the `disableJoystick=true`
> option if you use the port (or set the `EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK` compilation define).

This [image](https://w3c.github.io/gamepad/#remapping), represents the mapping returned by this API (as defined here):

![Gamepad Mapping](standard_gamepad.svg)

* If you use the `glfwGetJoystickAxes` and `glfwGetJoystickButtons` functions, you get exactly this mapping
* The function `glfwGetJoystickGUID` returns `Gamepad.mapping` and `glfwJoystickIsGamepad` returns `GLFW_TRUE` when the
  mapping is `"standard"`
* The function `glfwGetJoystickHats` maps the standard gamepad mapping to the `GLFW_HAT_XXX` bitfield

> #### Caution
> The function `glfwGetGamepadState` returns the same information that `glfwGetJoystickAxes` and
> `glfwGetJoystickButtons` but with the mapping specified by GLFW. Although very close to the `Gamepad` mapping,
> there are differences, so make sure you use the API that suits your needs. For example
> `GLFW_GAMEPAD_BUTTON_GUIDE` is 8 but the Gamepad guide is 16!
> Example:
> ```cpp
> int count;
> auto buttons = glfwGetJoystickButtons(jid, &count);
> auto isGuidePressed = count > 16 && buttons[16];
> // versus
> GLFWgamepadstate state;
> auto isGuidePressed = glfwGetGamepadState(jid, &state) == GLFW_TRUE && state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE];
> ```

## Clipboard support

In the context of the browser, getting access to the clipboard is a bit tricky:
- there are restrictions imposed by the browser for obvious security reasons
- the APIs to manage the clipboard are asynchronous

This implementation maintains an "internal" clipboard, one that is always available whether the browser
is authorized or not to access the external clipboard:
- `glfwSetClipboardString` always copy the string to the internal clipboard and tries to copy to the
  external one (which may fail)
- `glfwGetClipboardString` always return the contents of the internal clipboard
- `emscripten::glfw3::GetClipboardString` (resp. `emscripten_glfw_get_clipboard_string`) tries to fetch the 
   content of the external clipboard and if successful, also copies it to the internal one

Here is how you would access the external clipboard:

```cpp
// using global variable for the sake of this example
static auto kClipboardString = std::future<emscripten::glfw3::ClipboardString>{};

// main loop

// this happens on frame N
if(/* paste */) {
  kClipboardString = emscripten::glfw3::GetClipboardString();
}

// ...

// this happens on frame N + n (with n > 0 and usually n is 1)
if(kClipboardString.valid() &&
   kClipboardString.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
{
  // since the internal clipboard is updated with the external value, you can simply
  // call glfwGetClipboardString, which will contain:
  // - the value of the external clipboard if the call succeeded
  // - the value of the internal clipboard if it failed

  // auto clipboard = glfwGetClipboardString(window); 
  
  // clipboard handled, reset the future
  kClipboardString = {};
}
```

Using the C API:
```c
void GetClipboardHandler(void *iUserData, char const *iClipboardString, char const *iError)
{
  if(iError)
    // handle error-case
  else
    // handle clipboard string
}

// main loop
// userData = ...;
emscripten_glfw_get_clipboard_string(GetClipboardHandler, userData);
```

> #### Note
> The previous code bypasses the error case entirely. If you want to handle it yourself, you can do
> something like this:
> ```cpp
> if(kClipboardString.valid() &&
>    kClipboardString.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
>   auto clipboard = kClipboardString.get();
>   if(clipboard.hasValue()) {
>     // handle external clipboard value by calling clipboard.value()
>   } else {
>     // there was an error accessing the clipboard:
>     //   the error can be accessed with clipboard.error()
>     // you can use the internal clipboard instead:
>     //   glfwGetClipboardString()
>   }
>   // clipboard handled, reset the future
>   kClipboardString = {};
> }
> ```



## Extensions

This implementation offers a few extensions to the normal GLFW api necessary for this specific platform.

### CPP extensions

As explained previously, some CPP functions are defined in `<GLFW/emscripten_glfw3.h>`

> #### Note
> All functions are defined in the `emscripten::glfw3` namespace

| Function                      | Notes                                                                |
|-------------------------------|----------------------------------------------------------------------|
| `SetNextWindowCanvasSelector` | to specify the association window <-> canvas                         |
| `MakeCanvasResizable`         | to make the canvas resizable                                         |
| `UnmakeCanvasResizable`       | to revert `emscripten_glfw_make_canvas_resizable`                    |
| `IsWindowFullscreen`          | to check if the window is fullscreen                                 |
| `RequestFullscreen`           | to request fullscreen                                                |
| `GetClipboardString`          | to retrieve the content of the external clipboard (asynchronous API) |

### C extensions

Similar to the CPP functions, C functions are defined in `<GLFW/emscripten_glfw3.h>`, with a C-like syntax

| Function                                          | Notes                                                                |
|---------------------------------------------------|----------------------------------------------------------------------|
| `emscripten_glfw_set_next_window_canvas_selector` | to specify the association window <-> canvas                         |
| `emscripten_glfw_make_canvas_resizable`           | to make the canvas resizable                                         |
| `emscripten_glfw_unmake_canvas_resizable`         | to revert `emscripten_glfw_make_canvas_resizable`                    |
| `emscripten_glfw_is_window_fullscreen`            | to check if the window is fullscreen                                 |
| `emscripten_glfw_request_fullscreen`              | to request fullscreen                                                |
| `emscripten_glfw_get_clipboard_string`            | to retrieve the content of the external clipboard (asynchronous API) |

You can either include this file, or use an `extern "C" {}` section in your own code to define them

### Javascript extensions

This implementation adds the following functions to the `Module`:

| Function                                                          | Notes                                                                                                                                                 |
|-------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------|
| `requestFullscreen(lockPointer, resizeCanvas)`                    | Same function added by the other emscripten implementations (for backward compatibility purposes)                                                     |
| `glfwRequestFullscreen(target, lockPointer, resizeCanvas)`        | The version specific to this implementation with the additional `target` argument (can be a canvas selector, a `HTMLCanvasElement` or a `GLFWwindow`) |
| `glfwGetWindow(any)`                                              | Returns the `GLFWwindow` pointer associated to the canvas (`any` can be a canvas selector or a `HTMLCanvasElement`)                                   |
| `glfwGetCanvas(any)`                                              | Returns the canvas associated to the window (`any` can be a canvas selector or a `GLFWwindow`)                                                        |
| `glfwGetCanvasSelector(any)`                                      | Returns the canvas selector associated to the window (`any` can be a canvas selector or a `GLFWwindow`)                                               |
| `glfwMakeCanvasResizable(any, resizableSelector, handleSelector)` | Same functionality as `emscripten_glfw_make_canvas_resizable` (`any` can be a canvas selector or a `GLFWwindow` or a `HTMLCanvasElement`              |
| `glfwUnmakeCanvasResizable(any)`                                  | To revert `Module.glfwGetCanvasSelector`                                                                                                              |

In addition, this implementation will check if the function `Module.glfwOnWindowCreated(glfwWindow, selector)` is
defined in which case it will be called once the window is created. This allows writing code like this:

```javascript
Module = {
  // ...
  glfwOnWindowCreated: (glfwWindow, selector) => {
    if(selector === '#canvas2') {
      Module.glfwMakeCanvasResizable(glfwWindow, '#canvas2-container');
    }
  },
  // ...
};
```

## Implementation size

This implementation being in C++ and implementing far more features than the `library_glfw.js` emscripten
implementation, it has an impact on size.
As of initial release, I ran the following experiment on both implementations using [`example_minimal`](../examples/example_minimal)

| Mode              | `library_glfw.js`                      | This implementation                      | Delta |
|-------------------|----------------------------------------|------------------------------------------|-------|
| Debug             | js: 170775, wasm: 75789, total: 246564 | js: 99559, wasm: 4492007, total: 4591566 | 18.8x |
| Release           | js: 135433, wasm: 8448, total: 143881  | js: 81285, wasm: 80506, total: 161791    | 1.12x |
| Release (minimal) | -                                      | js: 79402, wasm: 71195, total: 150197    | 1.04x |

* From these numbers, and for obvious reasons, there is more wasm code than javascript code in this implementation
  (which is a good thing).
* Although the size is pretty terrible in `Debug` mode (almost a 19x size increase), in `Release`
  mode it is actually only a 12% increase which shows that wasm optimizes quite well :)
* The last entry in the table shows the same results when compiling with all _disable_ options turned on
  (`EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK`, `EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT` and
  `EMSCRIPTEN_GLFW3_DISABLE_WARNING`) for an even smaller footprint
* Lastly, `.wasm` files compress extremely well, so it is worth serving them compressed


## Implementation size (update)

![emscripten - 3.1.64](https://img.shields.io/badge/emscripten-3.1.64-blue)
![emscripten-glfw-3.4.0.20240627](https://img.shields.io/badge/emscripten--glfw-3.4.0.20240627-blue)

```text
> cd examples/example_minimal
# using library_glfw.js
> emcc -sUSE_GLFW=3 main.cpp -O2 -o /tmp/build/index.html
# using contrib.glfw3
> emcc --use-port=contrib.glfw3 main.cpp -O2 -o /tmp/build/index.html
# using contrib.glfw3 (minimal)
> emcc --use-port=contrib.glfw3:disableWarning=true:disableJoystick=true:disableMultiWindow=true main.cpp -O2 -o /tmp/build/index.html
```

| Mode              | `library_glfw.js`                      | This implementation                      | Delta |
|-------------------|----------------------------------------|------------------------------------------|-------|
| Release           | js: 104400, wasm: 13869, total: 118269 | js: 57024, wasm: 71214, total: 128238    | 1.08x |
| Release (minimal) | -                                      | js: 54736, wasm: 63098, total: 117834    | 0.99x |

> #### Note
> The good news is that emscripten is improving and this implementation is benefitting from it.

## Supported functions

This table contains the list of all the functions supported by this implementation with a few relevant notes

> ### Note
> GLFW 3.4 introduced the concept of a platform. This implementation adds the `GLFW_PLATFORM_EMSCRIPTEN` define
> in `empscriptem-glfw3.h`: the value is reserved (in a comment), but it is not defined in `glfw3.h`.

| Function                            | Notes                                                                                                                                                                                                                   |
|-------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `glfwCreateCursor`                  | All GLFW cursors are supported: uses the css style `cursor` on the canvas                                                                                                                                               |
| `glfwCreateStandardCursor`          | All GLFW cursors are supported                                                                                                                                                                                          |
| `glfwCreateWindow`                  | Support as many windows as you want: see section describing the association of a window and a canvas                                                                                                                    |
| `glfwDefaultWindowHints`            |                                                                                                                                                                                                                         |
| `glfwDestroyWindow`                 | Reverts all changes (event listeners, css style, ...) set by this library                                                                                                                                               |
| `glfwExtensionSupported`            | Same implementation as `library_glfw.js`                                                                                                                                                                                |
| `glfwFocusWindow`                   | Calls javascript `HTMLElement.focus()` on the canvas                                                                                                                                                                    | 
| `glfwGetClipboardString`            | See Clipboard Support section                                                                                                                                                                                           |
| `glfwGetCurrentContext`             | Only available if `glfwMakeContextCurrent` was called previously                                                                                                                                                        |
| `glfwGetCursorPos`                  | Hi DPI aware                                                                                                                                                                                                            |
| `glfwGetError`                      |                                                                                                                                                                                                                         |
| `glfwGetFramebufferSize`            | Hi DPI aware                                                                                                                                                                                                            |
| `glfwGetGamepadName`                | If gamepad, corresponds to `Gamepad.id` in javascript                                                                                                                                                                   |
| `glfwGetGamepadState`               | If gamepad, then `Gamepad.axes` and `Gamepad.buttons` (js) remapped for GLFW                                                                                                                                            |
| `glfwGetInputMode`                  | Supports only `GLFW_CURSOR`, `GLFW_STICKY_KEYS` and `GLFW_STICKY_MOUSE_BUTTONS`                                                                                                                                         |
| `glfwGetJoystickAxes`               | Corresponds to `Gamepad.axes` in javascript                                                                                                                                                                             |
| `glfwGetJoystickButtons`            | Corresponds to `Gamepad.buttons[x].value` in javascript                                                                                                                                                                 |
| `glfwGetJoystickGUID`               | Corresponds to `Gamepad.mapping` in javascript                                                                                                                                                                          |
| `glfwGetJoystickHats`               | If gamepad, corresponds to `Gamepad.buttons[x].pressed` in javascript remapped for GLFW                                                                                                                                 |
| `glfwGetJoystickName`               | Corresponds to `Gamepad.id` in javascript (limited to 64 characters due to emscripten limitation)                                                                                                                       |
| `glfwGetJoystickUserPointer`        |                                                                                                                                                                                                                         |
| `glfwGetKey`                        | Support `GLFW_STICKY_KEYS` as well                                                                                                                                                                                      |
| `glfwGetKeyName`                    | All names starts with `DOM_PK_`: example `DOM_PK_F1`.                                                                                                                                                                   |
| `glfwGetKeyScancode`                | See `KeyboardMapping.h` for actual mapping                                                                                                                                                                              |
| `glfwGetMonitorContentScale`        | Corresponds to `window.devicePixelRatio` in javascript                                                                                                                                                                  |
| `glfwGetMonitorName`                | The constant "Browser"                                                                                                                                                                                                  |
| `glfwGetMonitorPos`                 | Always 0/0                                                                                                                                                                                                              |
| `glfwGetMonitors`                   | Due to javascript restrictions, always only 1 monitor                                                                                                                                                                   |
| `glfwGetMonitorUserPointer`         |                                                                                                                                                                                                                         |
| `glfwGetMonitorWorkarea`            | 0x0 for position, `screen.width`x`screen.height` for size                                                                                                                                                               |
| `glfwGetMouseButton`                | Support `GLFW_STICKY_MOUSE_BUTTONS` as well                                                                                                                                                                             |
| `glfwGetPlatform`                   | `GLFW_PLATFORM_EMSCRIPTEN` (see note above)                                                                                                                                                                             |
| `glfwGetPrimaryMonitor`             | The single monitor returned in `glfwGetMonitors`                                                                                                                                                                        |
| `glfwGetTime`                       |                                                                                                                                                                                                                         |
| `glfwGetTimerFrequency`             | Always 1000                                                                                                                                                                                                             |
| `glfwGetTimerValue`                 | Corresponds to `performance.now()` in javascript                                                                                                                                                                        |
| `glfwGetVersion`                    |                                                                                                                                                                                                                         |
| `glfwGetVersionString`              | "Emscripten/WebAssembly GLFW " + GLFW version                                                                                                                                                                           |
| `glfwGetWindowAttrib`               | Supports for `GLFW_VISIBLE`, `GLFW_HOVERED`, `GLFW_FOCUSED`, `GLFW_FOCUS_ON_SHOW`, `GLFW_SCALE_FRAMEBUFFER`, `GLFW_SCALE_TO_MONITOR`, `GLFW_RESIZABLE`                                                                  |
| `glfwGetWindowContentScale`         | If HiDPI aware (`GLFW_SCALE_FRAMEBUFFER` is `GLFW_TRUE`), then current monitor scale, otherwise `1.0`                                                                                                                   |
| `glfwGetWindowFrameSize`            | Because a window is a canvas in this implementation, there is no edge => all 0                                                                                                                                          |
| `glfwGetWindowMonitor`              | The single monitor returned in `glfwGetMonitors`                                                                                                                                                                        |
| `glfwGetWindowOpacity`              |                                                                                                                                                                                                                         |
| `glfwGetWindowPos`                  | The position of the canvas in the page `getBoundingClientRect(canvas).x&y`                                                                                                                                              |
| `glfwGetWindowSize`                 | The size of the window/canvas                                                                                                                                                                                           |
| `glfwGetWindowTitle`                | The title of the window/canvas                                                                                                                                                                                          |
| `glfwGetWindowUserPointer`          |                                                                                                                                                                                                                         |
| `glfwHideWindow`                    | Set css property to `display: none` for the canvas                                                                                                                                                                      |
| `glfwInit`                          | Set a listener to monitor content scale change (ex: moving browser to different resolution screen)                                                                                                                      |
| `glfwInitHint`                      | `GLFW_PLATFORM` with value `GLFW_ANY_PLATFORM` or `GLFW_PLATFORM_EMSCRIPTEN`                                                                                                                                            |
| `glfwJoystickIsGamepad`             | Returns `GLFW_TRUE` when the joystick mapping (`Gamepad.mapping`) is "standard"                                                                                                                                         |
| `glfwJoystickPresent`               | Listens to `gamepadconnected` and `gamepaddisconnected` events to determine the presence.                                                                                                                               |
| `glfwMakeContextCurrent`            | Since this implementation supports multiple windows, it is important to call this if using OpenGL                                                                                                                       |
| `glfwPlatformSupported`             | `GLFW_TRUE` for `GLFW_PLATFORM_EMSCRIPTEN` only (see note above)                                                                                                                                                        |
| `glfwPollEvents`                    | Polls for joysticks only (can be disabled with `EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK` define)                                                                                                                              |
| `glfwRawMouseMotionSupported`       | Always `GLFW_FALSE` (not supported)                                                                                                                                                                                     |
| `glfwSetCharCallback`               | Uses `KeyboardEvent.key` to compute the proper codepoint                                                                                                                                                                |
| `glfwSetClipboardString`            | Uses `navigator.clipboard.writeText`. See Clipboard Support section.                                                                                                                                                    |
| `glfwSetCursor`                     | Uses css style `cursor: xxx` for the canvas                                                                                                                                                                             |
| `glfwSetCursorEnterCallback`        | Listeners to `mouseenter` and `mouseleave` events                                                                                                                                                                       |
| `glfwSetCursorPosCallback`          | Hi DPI aware                                                                                                                                                                                                            |
| `glfwSetErrorCallback`              |                                                                                                                                                                                                                         |
| `glfwSetFramebufferSizeCallback`    | Hi DPI aware                                                                                                                                                                                                            |
| `glfwSetInputMode`                  | Supports only `GLFW_CURSOR`, `GLFW_STICKY_KEYS` and `GLFW_STICKY_MOUSE_BUTTONS`                                                                                                                                         |
| `glfwSetJoystickCallback`           |                                                                                                                                                                                                                         |
| `glfwSetJoystickUserPointer`        |                                                                                                                                                                                                                         |
| `glfwSetKeyCallback`                |                                                                                                                                                                                                                         |
| `glfwSetMonitorCallback`            | Callback is never called                                                                                                                                                                                                |
| `glfwSetMonitorUserPointer`         |                                                                                                                                                                                                                         |
| `glfwSetMouseButtonCallback`        |                                                                                                                                                                                                                         |
| `glfwSetScrollCallback`             | Listens to `mousewheel` events                                                                                                                                                                                          |
| `glfwSetTime`                       |                                                                                                                                                                                                                         |
| `glfwSetWindowAspectRatio`          | Only works if the user is controlling the canvas size (spec does not define one way or another)                                                                                                                         |
| `glfwSetWindowAttrib`               | Supports for `GLFW_VISIBLE`, `GLFW_FOCUSED`, `GLFW_FOCUS_ON_SHOW`, `GLFW_SCALE_FRAMEBUFFER`, `GLFW_SCALE_TO_MONITOR`, `GLFW_RESIZABLE`                                                                                  |
| `glfwSetWindowContentScaleCallback` | Callback only called if Hi DPI aware (`GLFW_SCALE_FRAMEBUFFER` is `GLFW_TRUE`)                                                                                                                                          |
| `glfwSetWindowFocusCallback`        |                                                                                                                                                                                                                         |
| `glfwSetWindowOpacity`              | Uses css style `opacity: xxx` for the canvas                                                                                                                                                                            |
| `glfwSetWindowPosCallback`          | Uses top/left of the value returned by `getBoundingClientRect(canvas)`)                                                                                                                                                 |
| `glfwSetWindowRefreshCallback`      | Returns callback provided: callback is never called                                                                                                                                                                     |
| `glfwSetWindowShouldClose`          |                                                                                                                                                                                                                         |
| `glfwSetWindowSize`                 | Hi DPI Aware: set the size of the canvas (`canvas.width = size * scale`) + css style (`style.width = size`)                                                                                                             |
| `glfwSetWindowSizeCallback`         |                                                                                                                                                                                                                         |
| `glfwSetWindowSizeLimits`           | Only works if the user is controlling the canvas size (spec does not define one way or another)                                                                                                                         |
| `glfwSetWindowTitle`                | Corresponds to `document.title` in javascript                                                                                                                                                                           |
| `glfwSetWindowUserPointer`          |                                                                                                                                                                                                                         |
| `glfwShowWindow`                    | Removes css style `display: none` for the canvas                                                                                                                                                                        |
| `glfwSwapInterval`                  | Uses `emscripten_set_main_loop_timing`                                                                                                                                                                                  |
| `glfwTerminate`                     | Tries to properly cleanup everything that was set during the course of the app (listeners, css styles, ...)                                                                                                             |
| `glfwVulkanSupported`               | Always return `GLFW_FALSE`                                                                                                                                                                                              |
| `glfwWindowHint`                    | `GLFW_CLIENT_API`, `GLFW_SCALE_FRAMEBUFFER`, `GLFW_SCALE_TO_MONITOR`, `GLFW_FOCUS_ON_SHOW`, `GLFW_VISIBLE`, `GLFW_FOCUSED`, `GLFW_RESIZABLE`, `GLFW_ALPHA_BITS`, `GLFW_DEPTH_BITS`, `GLFW_STENCIL_BITS`, `GLFW_SAMPLES` |
| `glfwWindowHintString`              | None                                                                                                                                                                                                                    |
| `glfwWindowShouldClose`             |                                                                                                                                                                                                                         |

## Non-Supported functions

Note that these functions log a warning the first time they are called (which can be disabled via
`EMSCRIPTEN_GLFW3_DISABLE_WARNING` define) and are doing nothing, returning the most "sensible" value
(like `nullptr`) if there is an expected return value. Calling any of these will not break the library.

| Function                            | Notes                                                        |
|-------------------------------------|--------------------------------------------------------------|
| `glfwDestroyCursor`                 |                                                              |
| `glfwGetGammaRamp`                  | No access from javascript                                    |
| `glfwGetMonitorPhysicalSize`        | No access from javascript                                    |
| `glfwGetProcAddress`                | Implemented by emscripten                                    |
| `glfwGetRequiredInstanceExtensions` |                                                              |
| `glfwGetVideoMode`                  |                                                              |
| `glfwGetVideoModes`                 |                                                              |
| `glfwIconifyWindow`                 |                                                              |
| `glfwInitAllocator`                 | Due to javascript, memory cannot be managed                  |
| `glfwMaximizeWindow`                |                                                              |
| `glfwPostEmptyEvent`                |                                                              |
| `glfwRequestWindowAttention`        |                                                              |
| `glfwRestoreWindow`                 |                                                              |
| `glfwSetCharModsCallback`           | It is deprecated in GLFW                                     |
| `glfwSetCursorPos`                  | Javascript does not allow the cursor to be positioned        |
| `glfwSetDropCallback`               | Javascript only gives access to filename, so it is pointless |
| `glfwSetGamma`                      |                                                              |
| `glfwSetGammaRamp`                  |                                                              |
| `glfwSetWindowCloseCallback`        | There is no concept of "closing" a canvas                    |
| `glfwSetWindowIcon`                 | Icon could be mapped to favicon, but beyond 1.0 scope        |
| `glfwSetWindowIconifyCallback`      |                                                              |
| `glfwSetWindowMaximizeCallback`     |                                                              |
| `glfwSetWindowMonitor`              |                                                              |
| `glfwSetWindowPos`                  | There is no generic way to set a canvas position             |
| `glfwSwapBuffers`                   |                                                              |
| `glfwUpdateGamepadMappings`         |                                                              |
| `glfwWaitEvents`                    |                                                              |
| `glfwWaitEventsTimeout`             |                                                              |
