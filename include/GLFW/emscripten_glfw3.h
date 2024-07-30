/*
 * Copyright (c) 2024 pongasoft
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */

#ifndef EMSCRIPTEN_GLFW_EMSCRIPTEN_GLFW3_H
#define EMSCRIPTEN_GLFW_EMSCRIPTEN_GLFW3_H

#include <GLFW/glfw3.h>
#include <emscripten/em_types.h>

//------------------------------------------------------------------------
// CPP Interface
//------------------------------------------------------------------------
#ifdef __cplusplus

#include <future>
#include <string>
#include <optional>

namespace emscripten::glfw3 {

/**
 * Value returned by `GetClipboardString`: encapsulates the fact that the API can return an error (due to restrictions
 * imposed by browser when accessing the clipboard).
 *
 * Note: Accessing `value()` when there is an error simply returns the internal clipboard
 */
struct ClipboardString
{
  bool hasValue() const { return fValue.has_value(); }
  bool hasError() const { return fError.has_value(); }
  std::string value() const { return hasValue() ? *fValue : glfwGetClipboardString(nullptr); }
  std::string value_or(std::string const &iValueOnError) const { return fValue.value_or(iValueOnError); }
  std::string const &error() const { return *fError; }

  static ClipboardString fromValue(std::string iValue);
  static ClipboardString fromError(std::string iError);

private:
  ClipboardString(std::optional<std::string> iValue, std::optional<std::string> iError);

  std::optional<std::string> fValue{};
  std::optional<std::string> fError{};
};

/**
 * Helper class to make the code easier to write
 *
 * ```cpp
 * using namespace emscripten::glfw3;
 * FutureClipboardString clipboard{};
 *
 * // on paste event
 * clipboard = GetClipboardString();
 *
 * // on each frame
 * if(clipboard)
 * {
 *   clipboard.fetchValue();
 * }
 * ```
 */
struct FutureClipboardString
{
  FutureClipboardString &operator=(std::future<ClipboardString> iFuture) { fFuture = std::move(iFuture); return *this; };
  explicit operator bool() const { return fFuture.valid() && fFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }
  ClipboardString fetch() { auto res = fFuture.get(); fFuture = {}; return res; }
  std::string fetchValue() { return fetch().value(); }
private:
  std::future<ClipboardString> fFuture{};
};

/**
 * The `glfwGetClipboardString` function does not work well in a browser environment due to the fact that the
 * Javascript API to access the clipboard is asynchronous. This function provides an alternative way to retrieve
 * the clipboard in this environment.
 *
 * Note 1: Due to the restrictions imposed by browsers in accessing the clipboard, the value returned by the future
 *         can contain an error. Note that the future itself will never be in error (no exception).
 * Note 2: This implementation also maintains an "internal" clipboard, one that is always available whether the browser
 *         is authorized or not to access the external clipboard:
 *         - `glfwSetClipboardString` always copy the string to the internal clipboard and tries to copy to the
 *            external one (which may fail)
 *         - `glfwGetClipboardString` always return the contents of the internal clipboard
 *         - `emscripten::glfw3::GetClipboardString` tries to fetch the content of the external clipboard and if
 *           successful, also copies it to the internal one
 */
std::future<ClipboardString> GetClipboardString();

/**
 * Before calling `glfwCreateWindow` you can communicate to the library which canvas to use by calling this function.
 * Conceptually this is similar to a window hint (must be called **prior** to creating the window).
 *
 * The parameter `canvasSelector` should be a (css path) selector referring to the canvas (ex: `#canvas`).
 *
 * By default, and to be backward compatible with the other emscripten ports, if this function is
 * not called, the library will use `Module["canvas"]`.
 *
 * If you want to create more than one window, you **must** call this function to specify which canvas to associate
 * to the window otherwise you will get a duplicate canvas error when creating the windows. */
void SetNextWindowCanvasSelector(std::string_view canvasSelector);

/**
 * If you want the canvas (= window) size to be adjusted dynamically by the user you can call this
 * convenient function. Although you can implement this functionality yourself, the implementation can
 * be tricky to get right.
 *
 * Since this library takes charge of the size of the canvas, the idea behind this function is to specify which
 * other (html) element dictates the size of the canvas. The parameter `canvasResizeSelector` defines the
 * (css path) selector to this element.
 *
 * The 3 typical uses cases are:
 *
 * 1. the canvas fills the entire browser window, in which case the parameter `canvasResizeSelector` should simply
 *    be set to "window" and the `handleSelector` is `std::nullopt` (which is the default). This use case can be
 *    found in application like ImGui where the canvas is the window.
 *
 *    Example code:
 *
 *    ```html
 *    <canvas id="canvas1"></canvas>
 *    ```
 *
 *    ```cpp
 *    emscripten::glfw3::SetNextWindowCanvasSelector("#canvas1");
 *    auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
 *    emscripten::glfw3::MakeCanvasResizable(window, "window");
 *    ```
 *
 * 2. the canvas is inside a `div`, in which case the `div` acts as a "container" and the `div` size is defined by
 *    CSS rules, like for example: `width: 85%` so that when the page/browser gets resized, the `div` is resized
 *    automatically, which then triggers the canvas to be resized. In this case, the parameter `canvasResizeSelector`
 *    is the (css path) selector to this `div` and `handleSelector` is `std::nullopt` (default).
 *
 *    Example code:
 *
 *    ```html
 *    <style>#canvas1-container { width: 85%; height: 85% }</style>
 *    <div id="canvas1-container"><canvas id="canvas1"></canvas></div>
 *    ```
 *
 *    ```cpp
 *    emscripten::glfw3::SetNextWindowCanvasSelector("#canvas1");
 *    auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
 *    emscripten::glfw3::MakeCanvasResizable(window, "canvas1-container");
 *    ```
 *
 * 3. same as 2. but the `div` is made resizable dynamically via a little "handle" (which ends up behaving like a
 *    normal desktop window).
 *
 *    Example code:
 *
 *    ```html
 *    <style>#canvas1-container { position: relative; <!-- ... --> }</style>
 *    <style>#canvas1-handle { position: absolute; bottom: 0; right: 0; background-color: #444444; width: 10px; height: 10px; cursor: nwse-resize; }</style>
 *    <div id="canvas1-container"><div id="canvas1-handle" class="handle"></div><canvas id="canvas1"></canvas></div>
 *    ```
 *
 *    ```cpp
 *    emscripten::glfw3::SetNextWindowCanvasSelector("#canvas1");
 *    auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
 *    emscripten::glfw3::MakeCanvasResizable(window, "canvas1-container", "canvas1-handle");
 *    ```
 *
 * Note that there is an equivalent call added to `Module` that can be invoked from javascript:
 * `Module.glfwMakeCanvasResizable(...)`.
 *
 * @return `EMSCRIPTEN_RESULT_SUCCESS` if there was no issue, or an emscripten error code
 *          otherwise (ex: a selector not referring to an existing element) */
int MakeCanvasResizable(GLFWwindow *window,
                        std::string_view canvasResizeSelector,
                        std::optional<std::string_view> handleSelector = std::nullopt);

/**
 * The opposite of `MakeCanvasResizable`
 *
 * @return `EMSCRIPTEN_RESULT_SUCCESS` if there was no issue, or an emscripten error code
 *          otherwise (ex: not a valid window) */
int UnmakeCanvasResizable(GLFWwindow *window);

/**
 * Returns `true` if the window is fullscreen, `false` otherwise */
bool IsWindowFullscreen(GLFWwindow *window);

/**
 * Requests the window to go fullscreen. Note that due to browser restrictions, this function should only
 * be called from a user generated event (like a keyboard event or a mouse button press).
 *
 * Note that there is an equivalent call added to `Module` that can be invoked from javascript:
 * `Module.glfwRequestFullscreen(...)`.
 *
 * @param window which window to go fullscreen
 * @param lockPointer whether to lock the pointer or not
 * @param resizeCanvas whether to resize the canvas to match the fullscreen size or not
 * @return `EMSCRIPTEN_RESULT_SUCCESS` if there was no issue, or an emscripten error code otherwise */
int RequestFullscreen(GLFWwindow *window, bool lockPointer, bool resizeCanvas);

} // namespace emscripten::glfw3

#endif // __cplusplus

//------------------------------------------------------------------------
// C Interface
//------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#define GLFW_PLATFORM_EMSCRIPTEN 0x00060006

/**
 * Before calling `glfwCreateWindow` you can communicate to the library which canvas to use by calling this function.
 * Conceptually this is similar to a window hint (must be called **prior** to creating the window).
 *
 * The parameter `canvasSelector` should be a (css path) selector referring to the canvas (ex: `#canvas`).
 *
 * By default, and to be backward compatible with the other emscripten ports, if this function is
 * not called, the library will use `Module["canvas"]`.
 *
 * If you want to create more than one window, you **must** call this function to specify which canvas to associate
 * to the window otherwise you will get a duplicate canvas error when creating the windows. */
void emscripten_glfw_set_next_window_canvas_selector(char const *canvasSelector);

/**
 * If you want the canvas (= window) size to be adjusted dynamically by the user you can call this
 * convenient function. Although you can implement this functionality yourself, the implementation can
 * be tricky to get right.
 *
 * Since this library takes charge of the size of the canvas, the idea behind this function is to specify which
 * other (html) element dictates the size of the canvas. The parameter `canvasResizeSelector` defines the
 * (css path) selector to this element.
 *
 * The 3 typical uses cases are:
 *
 * 1. the canvas fills the entire browser window, in which case the parameter `canvasResizeSelector` should simply
 *    be set to "window" and the `handleSelector` is `nullptr`. This use case can be found in application like ImGui
 *    where the canvas is the window.
 *
 *    Example code:
 *
 *    ```html
 *    <canvas id="canvas1"></canvas>
 *    ```
 *
 *    ```cpp
 *    emscripten_glfw_set_next_window_canvas_selector("#canvas1");
 *    auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
 *    emscripten_glfw_make_canvas_resizable(window, "window", nullptr);
 *    ```
 *
 * 2. the canvas is inside a `div`, in which case the `div` acts as a "container" and the `div` size is defined by
 *    CSS rules, like for example: `width: 85%` so that when the page/browser gets resized, the `div` is resized
 *    automatically, which then triggers the canvas to be resized. In this case, the parameter `canvasResizeSelector`
 *    is the (css path) selector to this `div` and `handleSelector` is `nullptr`.
 *
 *    Example code:
 *
 *    ```html
 *    <style>#canvas1-container { width: 85%; height: 85% }</style>
 *    <div id="canvas1-container"><canvas id="canvas1"></canvas></div>
 *    ```
 *
 *    ```cpp
 *    emscripten_glfw_set_next_window_canvas_selector("#canvas1");
 *    auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
 *    emscripten_glfw_make_canvas_resizable(window, "#canvas1-container", nullptr);
 *    ```
 *
 * 3. same as 2. but the `div` is made resizable dynamically via a little "handle" (which ends up behaving like a
 *    normal desktop window).
 *
 *    Example code:
 *
 *    ```html
 *    <style>#canvas1-container { position: relative; <!-- ... --> }</style>
 *    <style>#canvas1-handle { position: absolute; bottom: 0; right: 0; background-color: #444444; width: 10px; height: 10px; cursor: nwse-resize; }</style>
 *    <div id="canvas1-container"><div id="canvas1-handle" class="handle"></div><canvas id="canvas1"></canvas></div>
 *    ```
 *
 *    ```cpp
 *    emscripten_glfw_set_next_window_canvas_selector("#canvas1");
 *    auto window = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
 *    emscripten_glfw_make_canvas_resizable(window, "#canvas1-container", "canvas1-handle");
 *    ```
 *
 * Note that there is an equivalent call added to `Module` that can be invoked from javascript:
 * `Module.glfwMakeCanvasResizable(...)`.
 *
 * @return `EMSCRIPTEN_RESULT_SUCCESS` if there was no issue, or an emscripten error code
 *          otherwise (ex: a selector not referring to an existing element) */
int emscripten_glfw_make_canvas_resizable(GLFWwindow *window,
                                          char const *canvasResizeSelector,
                                          char const *handleSelector);

/**
 * The opposite of `emscripten_glfw_make_canvas_resizable`
 *
 * @return `EMSCRIPTEN_RESULT_SUCCESS` if there was no issue, or an emscripten error code
 *          otherwise (ex: not a valid window) */
int emscripten_glfw_unmake_canvas_resizable(GLFWwindow *window);

/**
 * Returns `EM_TRUE` if the window is fullscreen, `EM_FALSE` otherwise */
EM_BOOL emscripten_glfw_is_window_fullscreen(GLFWwindow *window);

/**
 * Requests the window to go fullscreen. Note that due to browser restrictions, this function should only
 * be called from a user generated event (like a keyboard event or a mouse button press).
 *
 * Note that there is an equivalent call added to `Module` that can be invoked from javascript:
 * `Module.glfwRequestFullscreen(...)`.
 *
 * @param window which window to go fullscreen
 * @param lockPointer whether to lock the pointer or not
 * @param resizeCanvas whether to resize the canvas to match the fullscreen size or not
 * @return `EMSCRIPTEN_RESULT_SUCCESS` if there was no issue, or an emscripten error code otherwise */
int emscripten_glfw_request_fullscreen(GLFWwindow *window, EM_BOOL lockPointer, EM_BOOL resizeCanvas);

/**
 * The `glfwGetClipboardString` function does not work well in a browser environment due to the fact that the
 * Javascript API to access the clipboard is asynchronous. This function provides an alternative way to retrieve
 * the clipboard in this environment.
 *
 * Note 1: Due to the restrictions imposed by browsers in accessing the clipboard, `error` will contain the
 *         error message and `clipboardString` will be null when there is an error.
 * Note 2: This implementation also maintains an "internal" clipboard, one that is always available whether the browser
 *         is authorized or not to access the external clipboard:
 *         - `glfwSetClipboardString` always copy the string to the internal clipboard and tries to copy to the
 *            external one (which may fail)
 *         - `glfwGetClipboardString` always return the contents of the internal clipboard
 *         - `emscripten_glfw_get_clipboard_string` tries to fetch the content of the external clipboard and if
 *           successful, also copies it to the internal one
 */
typedef void (* emscripten_glfw_clipboard_string_fun)(void *userData, char const *clipboardString, char const *error);
void emscripten_glfw_get_clipboard_string(emscripten_glfw_clipboard_string_fun callback, void *userData);

#ifdef __cplusplus
}
#endif

#endif //EMSCRIPTEN_GLFW_EMSCRIPTEN_GLFW3_H
