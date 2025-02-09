/*
 * Copyright (c) 2023 pongasoft
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

#include "Context.h"
#include <emscripten.h>
#include <emscripten/html5.h>

#include "ErrorHandler.h"

#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
#include "Joystick.h"
#endif

extern "C" {
using ScaleChangeCallback = void (*)(emscripten::glfw3::Context *);
using WindowResizeCallback = void (*)(emscripten::glfw3::Context *, GLFWwindow *, int, int);
using KeyboardCallback = bool (*)(emscripten::glfw3::Context *, bool, char const *, char const *, bool, int, int);
using ClipboardStringCallback = void (*)(emscripten::glfw3::Context *, char const *, char const *);
using RequestFullscreen = int (*)(GLFWwindow *, EM_BOOL, EM_BOOL);
using ErrorHandler = void (*)(int, char const *);


void emscripten_glfw3_context_init(emscripten::glfw3::Context *iContext,
                                   float iScale,
                                   ScaleChangeCallback, WindowResizeCallback, KeyboardCallback, ClipboardStringCallback, RequestFullscreen, ErrorHandler);
void emscripten_glfw3_context_destroy();
bool emscripten_glfw3_context_is_any_element_focused();
bool emscripten_glfw3_context_is_extension_supported(char const *iExtension);
void emscripten_glfw3_context_set_title(char const *iTitle);
double emscripten_glfw3_context_get_now();
GLFWwindow *emscripten_glfw3_context_get_fullscreen_window();
GLFWwindow *emscripten_glfw3_context_get_pointer_lock_window();
int emscripten_glfw3_window_init(GLFWwindow *iWindow, char const *iCanvasSelector);
void emscripten_glfw3_window_on_created(GLFWwindow *iWindow);
void emscripten_glfw3_context_set_clipboard_string(char const *iContent);
void emscripten_glfw3_context_open_url(char const *, char const *);
bool emscripten_glfw3_context_is_runtime_platform_apple();
void emscripten_glfw3_create_custom_cursor(GLFWcursor *iCursor, int iWidth, int iHeight, unsigned char *iPixels);
void emscripten_glfw3_destroy_custom_cursor(GLFWcursor *iCursor);
}

namespace emscripten::glfw3 {

static ErrorHandler &kErrorHandler = ErrorHandler::instance();

//------------------------------------------------------------------------
// Context::init
//------------------------------------------------------------------------
std::unique_ptr<Context> Context::init()
{
  return std::unique_ptr<Context>(new Context{});
}

//------------------------------------------------------------------------
// ContextScaleChangeCallback
//------------------------------------------------------------------------
void ContextScaleChangeCallback(Context *iContext)
{
  iContext->onScaleChange();
}

//------------------------------------------------------------------------
// ContextWindowResizeCallback
//------------------------------------------------------------------------
void ContextWindowResizeCallback(Context *iContext, GLFWwindow *iWindow, int iWidth, int iHeight)
{
  iContext->onWindowResize(iWindow, iWidth, iHeight);
}

//------------------------------------------------------------------------
// ContextKeyboardCallback
//------------------------------------------------------------------------
bool ContextKeyboardCallback(emscripten::glfw3::Context *iContext,
                             bool iKeyDown,
                             char const *iCode,
                             char const *iKey,
                             bool iRepeat,
                             int iCodepoint,
                             int iModifierBits)
{
  Keyboard::Event event{iCode, iKey, iRepeat, iCodepoint, iModifierBits};
  if(iKeyDown)
    return iContext->onKeyDown(event);
  else
    return iContext->onKeyUp(event);
}

//------------------------------------------------------------------------
// ContextClipboardStringCallback
//------------------------------------------------------------------------
void ContextClipboardStringCallback(Context *iContext, char const *iText, char const *iError)
{
  iContext->onClipboard(iText, iError);
}

//------------------------------------------------------------------------
// ContextErrorHandler
//------------------------------------------------------------------------
void ContextErrorHandler(int iErrorCode, char const *iErrorMessage)
{
  kErrorHandler.logError(iErrorCode, iErrorMessage);
}

//------------------------------------------------------------------------
// Context::Context
//------------------------------------------------------------------------
Context::Context()
{
  fScale = static_cast<float>(emscripten_get_device_pixel_ratio());
  emscripten_glfw3_context_init(this,
                                fScale,
                                ContextScaleChangeCallback,
                                ContextWindowResizeCallback,
                                ContextKeyboardCallback,
                                ContextClipboardStringCallback,
                                emscripten_glfw_request_fullscreen,
                                ContextErrorHandler);
  addOrRemoveEventListeners(true);
}

//------------------------------------------------------------------------
// Context::~Context
//------------------------------------------------------------------------
Context::~Context()
{
  terminate();
  addOrRemoveEventListeners(false);
  emscripten_glfw3_context_destroy();
}


//------------------------------------------------------------------------
// Context::terminate
//------------------------------------------------------------------------
void Context::terminate()
{
  fCurrentWindow = nullptr;

#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  for(auto &w: fWindows)
    w->destroy();

  fWindows.clear();
#else
  if(fSingleWindow)
    fSingleWindow->destroy();
  fSingleWindow = nullptr;
#endif
}

//------------------------------------------------------------------------
// Context::addOrRemoveEventListeners
//------------------------------------------------------------------------
void Context::addOrRemoveEventListeners(bool iAdd)
{
  if(iAdd)
  {
    // fOnMouseMove
    fOnMouseMove
      .target(EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
      .listener([this](int iEventType, const EmscriptenMouseEvent *iEvent) {
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
        for(auto &w: fWindows)
          w->onGlobalMouseMove(iEvent);
#else
        if(fSingleWindow)
          fSingleWindow->onGlobalMouseMove(iEvent);
#endif
        return true;
      })
      .add(emscripten_set_mousemove_callback_on_thread);

    // fOnMouseButtonUp
    fOnMouseButtonUp
      .target(EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
      .listener([this](int iEventType, const EmscriptenMouseEvent *iEvent) {
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
        bool handled = false;
        for(auto &w: fWindows)
          handled |= w->onMouseButtonUp(iEvent);
        return handled;
#else
        return fSingleWindow && fSingleWindow->onMouseButtonUp(iEvent);
#endif
      })
      .add(emscripten_set_mouseup_callback_on_thread);

    // fOnFullscreenChange
    fOnFullscreenChange
      .target(EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
      .listener([this](int iEventType, EmscriptenFullscreenChangeEvent const *iEvent) {
        return toCBool(iEvent->isFullscreen) ? onEnterFullscreen(iEvent) : onExitFullscreen();
      })
      .add(emscripten_set_fullscreenchange_callback_on_thread);

    // fOnPointerLockChange
    fOnPointerLockChange
      .target(EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
      .listener([this](int iEventType, EmscriptenPointerlockChangeEvent const *iEvent) {
        return toCBool(iEvent->isActive) ? onPointerLock(iEvent) : onPointerUnlock();
      })
      .add(emscripten_set_pointerlockchange_callback_on_thread);

    // fOnPointerLockError
    fOnPointerLockError
      .target(EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
      .listener([this](int iEventType, void const *iEvent) {
        kErrorHandler.logError(GLFW_PLATFORM_ERROR,
                               "Error while requesting pointerLock (make sure you call this API from a user initiated event, like a mouse click)");
        fPointerLockRequest = std::nullopt;
        return true;
      })
      .add(emscripten_set_pointerlockerror_callback_on_thread);

    // fOnTouchStart
    fOnTouchStart
      .target(EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
      .listener([this](int iEventType, const EmscriptenTouchEvent *iEvent) { return onTouchStart(nullptr, iEvent); })
      .add(emscripten_set_touchstart_callback_on_thread);

    // fOnTouchMove
    fOnTouchMove
      .target(EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
      .listener([this](int iEventType, const EmscriptenTouchEvent *iEvent) { return onTouchMove(iEvent); })
      .add(emscripten_set_touchmove_callback_on_thread);

    // fOnTouchCancel
    fOnTouchCancel
      .target(EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
      .listener([this](int iEventType, const EmscriptenTouchEvent *iEvent) { return onTouchEnd(iEvent); })
      .add(emscripten_set_touchcancel_callback_on_thread);

    // fOnTouchEnd
    fOnTouchEnd
      .target(EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
      .listener([this](int iEventType, const EmscriptenTouchEvent *iEvent) { return onTouchEnd(iEvent); })
      .add(emscripten_set_touchend_callback_on_thread);

    // gamepad/joystick
#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
    fOnGamepadConnected
      .listener([this](int iEventType, EmscriptenGamepadEvent const *iEvent) { return onGamepadConnectionChange(iEvent); })
      .add(emscripten_set_gamepadconnected_callback_on_thread);

    fOnGamepadDisconnected
      .listener([this](int iEventType, EmscriptenGamepadEvent const *iEvent) { return onGamepadConnectionChange(iEvent); })
      .add(emscripten_set_gamepaddisconnected_callback_on_thread);
#endif
  }
  else
  {
    fOnMouseMove.remove();
    fOnMouseButtonUp.remove();
    fOnFullscreenChange.remove();
    fOnPointerLockChange.remove();
    fOnPointerLockError.remove();

    fOnTouchStart.remove();
    fOnTouchMove.remove();
    fOnTouchCancel.remove();
    fOnTouchEnd.remove();

#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
    fOnGamepadConnected.remove();
    fOnGamepadDisconnected.remove();
#endif
  }
}

//------------------------------------------------------------------------
// Context::onKeyDown
//------------------------------------------------------------------------
bool Context::onKeyDown(Keyboard::Event const &iEvent)
{
  bool handled = false;
  auto w = findFocusedOrSingleWindow();
  if(w && (w->isFocused() || !emscripten_glfw3_context_is_any_element_focused()))
    handled |= w->onKeyDown(iEvent, fBrowserKeyCallback);
  return handled;
}

//------------------------------------------------------------------------
// Context::onKeyUp
//------------------------------------------------------------------------
bool Context::onKeyUp(Keyboard::Event const &iEvent)
{
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  bool handled = false;
  for(auto &w: fWindows)
    handled |= w->onKeyUp(iEvent, fBrowserKeyCallback);
  return handled;
#else
  return fSingleWindow && fSingleWindow->onKeyUp(iEvent, fBrowserKeyCallback);
#endif
}

//------------------------------------------------------------------------
// Context::computeWindowPos
//------------------------------------------------------------------------
void Context::computeWindowPos()
{
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  for(auto &w: fWindows)
  {
    w->computePos(!isTrackingTouch());
  }
#else
  if(fSingleWindow)
    fSingleWindow->computePos(!isTrackingTouch());
#endif

}

//------------------------------------------------------------------------
// Context::onScaleChange
//------------------------------------------------------------------------
void Context::onScaleChange()
{
  fScale = static_cast<float>(emscripten_get_device_pixel_ratio());
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  for(auto &w: fWindows)
  {
    w->setMonitorScale(fScale);
  }
#else
  if(fSingleWindow)
    fSingleWindow->setMonitorScale(fScale);
#endif
}

//------------------------------------------------------------------------
// Context::onWindowResize
//------------------------------------------------------------------------
void Context::onWindowResize(GLFWwindow *iWindow, int iWidth, int iHeight)
{
  auto window = findWindow(iWindow);
  if(window)
    window->resize({iWidth, iHeight});
}

//------------------------------------------------------------------------
// Context::requestFullscreen
//------------------------------------------------------------------------
int Context::requestFullscreen(GLFWwindow *iWindow, bool iLockPointer, bool iResizeCanvas)
{
  // Per spec: If calling requestPointerLock() with requestFullscreen(), the requestPointerLock() must be
  // called first, because the requestFullscreen() will consume the state of transient activation.
  if(iLockPointer)
  {
    if(auto res = requestPointerLock(iWindow); res != EMSCRIPTEN_RESULT_SUCCESS)
      return res;
  }

  auto window = iWindow ? getWindow(iWindow) : findFocusedOrSingleWindow();

  if(window)
  {
    fFullscreenRequest = window->requestFullscreen(iResizeCanvas);
    auto res = emscripten_request_fullscreen(window->getCanvasSelector(), false);
    if(res != EMSCRIPTEN_RESULT_SUCCESS)
    {
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Error while requesting fullscreen on [%s] (make sure you call this API from a user initiated event, like a mouse click)", window->getCanvasSelector());
      fFullscreenRequest = std::nullopt;
    }
    return res;
  }
  else
  {
    kErrorHandler.logError(GLFW_INVALID_VALUE, "Error while requesting fullscreen: no window");
    return EMSCRIPTEN_RESULT_INVALID_TARGET;
  }
}

//------------------------------------------------------------------------
// Context::requestPointerLock
//------------------------------------------------------------------------
int Context::requestPointerLock(GLFWwindow *iWindow)
{
  auto window = iWindow ? getWindow(iWindow) : findFocusedOrSingleWindow();

  if(window)
  {
    fPointerLockRequest = {iWindow};
    if(auto res = emscripten_request_pointerlock(window->getCanvasSelector(), false); res != EMSCRIPTEN_RESULT_SUCCESS)
    {
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Error while requesting pointerLock for [%s] (make sure you call this API from a user initiated event, like a mouse click)", window->getCanvasSelector());
      fPointerLockRequest = std::nullopt;
      return res;
    }
    return EMSCRIPTEN_RESULT_SUCCESS;
  }
  else
  {
    kErrorHandler.logError(GLFW_INVALID_VALUE, "Error while requesting pointerLock: no window");
    return EMSCRIPTEN_RESULT_INVALID_TARGET;
  }
}

//------------------------------------------------------------------------
// Context::requestPointerUnlock
//------------------------------------------------------------------------
void Context::requestPointerUnlock(GLFWwindow *iWindow, glfw_cursor_mode_t iCursorMode)
{
  auto window = iWindow ? getWindow(iWindow) : findFocusedOrSingleWindow();

  if(window)
  {
    fPointerUnlockRequest = {iWindow, iCursorMode};
    if(emscripten_exit_pointerlock() != EMSCRIPTEN_RESULT_SUCCESS)
    {
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Error while exiting pointerLock (make sure you call this API from a user initiated event, like a mouse click)");
      fPointerUnlockRequest = std::nullopt;
    }
  }
}

//------------------------------------------------------------------------
// Context::onEnterFullscreen
//------------------------------------------------------------------------
bool Context::onEnterFullscreen(EmscriptenFullscreenChangeEvent const *iEvent)
{
  auto fullscreenRequest = std::exchange(fFullscreenRequest, std::nullopt);

  // which window is being targeted
  if(auto window = findWindow(emscripten_glfw3_context_get_fullscreen_window()); window)
  {
    if(!fullscreenRequest || fullscreenRequest->fWindow != window->asOpaquePtr())
    {
      // due to the asynchronous nature of this callback, this could technically happen so handling it to be on
      // the safe side
      kErrorHandler.logWarning("Out of order fullscreen request");
      fullscreenRequest = {window->asOpaquePtr(), true};
    }
    window->onEnterFullscreen(fullscreenRequest->fResizeCanvas ?
                              std::optional<Vec2<int>>{{iEvent->screenWidth, iEvent->screenHeight}} :
                              std::nullopt);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------
// Context::onExitFullscreen
//------------------------------------------------------------------------
bool Context::onExitFullscreen()
{
  bool res = false;

  // only 1 window should be in fullscreen
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  for(auto &w: fWindows)
    res |= w->onExitFullscreen();
#else
  res = fSingleWindow && fSingleWindow->onExitFullscreen();
#endif

  return res;
}

//------------------------------------------------------------------------
// Context::onPointerLock
//------------------------------------------------------------------------
bool Context::onPointerLock(EmscriptenPointerlockChangeEvent const *iEvent)
{
  auto lockPointerRequest = std::exchange(fPointerLockRequest, std::nullopt);

  if(auto window = findWindow(emscripten_glfw3_context_get_pointer_lock_window()); window)
  {
    if(!lockPointerRequest || lockPointerRequest->fWindow != window->asOpaquePtr())
    {
      // due to the asynchronous nature of this callback, this could technically happen so handling it to be on
      // the safe side
      kErrorHandler.logWarning("Out of order pointerLock request");
    }
    window->onPointerLock();
    return true;
  }

  return false;
}

//------------------------------------------------------------------------
// Context::onPointerUnlock
//------------------------------------------------------------------------
bool Context::onPointerUnlock()
{
  // this async callback does not contain any information, so we assume it is the one coming from the request
  if(auto unlockPointerRequest = std::exchange(fPointerUnlockRequest, std::nullopt); unlockPointerRequest)
  {
    if(auto window = findWindow(unlockPointerRequest->fWindow); window)
    {
      window->onPointerUnlock(unlockPointerRequest->fCursorMode);
      return true;
    }
  }

  // no request nor window matching the request => send to all windows
  bool res = false;

#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  for(auto &w: fWindows)
    res |= w->onPointerUnlock(std::nullopt);
#else
  res = fSingleWindow && fSingleWindow->onPointerUnlock(std::nullopt);
#endif


  return res;
}

//------------------------------------------------------------------------
// Context::findTouchPoint
//------------------------------------------------------------------------
EmscriptenTouchPoint const *Context::findTouchPoint(EmscriptenTouchEvent const *iEvent) const
{
  if(!isTrackingTouch())
    return nullptr;

  for(auto i = 0; i < iEvent->numTouches; i++)
  {
    auto const touchPoint = &iEvent->touches[i];

    // no change => skip
    if(!touchPoint->isChanged)
      continue;

    if(touchPoint->identifier == *fTouchPointId)
      return touchPoint;
  }

  return nullptr;
}

namespace impl {

//------------------------------------------------------------------------
// impl::findFirstTouchPoint
//------------------------------------------------------------------------
EmscriptenTouchPoint const *findFirstTouchPoint(EmscriptenTouchEvent const *iEvent)
{
  for(auto i = 0; i < iEvent->numTouches; i++)
  {
    auto const touchPoint = &iEvent->touches[i];

    // no change => skip
    if(!touchPoint->isChanged)
      continue;

    return touchPoint;
  }

  return nullptr;
}

}

//------------------------------------------------------------------------
// Context::onTouchStart
//------------------------------------------------------------------------
bool Context::onTouchStart(GLFWwindow *iOriginWindow, EmscriptenTouchEvent const *iEvent)
{
  // we don't handle multitouch
  if(isTrackingTouch())
    return false;

  auto touchPoint = impl::findFirstTouchPoint(iEvent);

  if(touchPoint)
  {
    fTouchPointId = touchPoint->identifier;
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
    for(auto &w: fWindows)
      w->onGlobalTouchStart(iOriginWindow, touchPoint);
#else
    if(fSingleWindow)
      fSingleWindow->onGlobalTouchStart(iOriginWindow, touchPoint);
#endif
    return iOriginWindow != nullptr;
  }

  return false;
}

//------------------------------------------------------------------------
// Context::onTouchMove
//------------------------------------------------------------------------
bool Context::onTouchMove(EmscriptenTouchEvent const *iEvent)
{
  auto touchPoint = findTouchPoint(iEvent);
  if(touchPoint)
  {
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
    for(auto &w: fWindows)
      w->onGlobalTouchMove(touchPoint);
#else
    if(fSingleWindow)
      fSingleWindow->onGlobalTouchMove(touchPoint);
#endif
  }
  return false;
}

//------------------------------------------------------------------------
// Context::onTouchEnd
//------------------------------------------------------------------------
bool Context::onTouchEnd(EmscriptenTouchEvent const *iEvent)
{
  auto touchPoint = findTouchPoint(iEvent);
  if(touchPoint)
  {
    fTouchPointId = std::nullopt;
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
    for(auto &w: fWindows)
      w->onGlobalTouchEnd(touchPoint);
#else
    if(fSingleWindow)
      fSingleWindow->onGlobalTouchEnd(touchPoint);
#endif
  }
  return false;
}

#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
//------------------------------------------------------------------------
// Context::onGamepadConnectionChange
//------------------------------------------------------------------------
bool Context::onGamepadConnectionChange(EmscriptenGamepadEvent const *iEvent)
{
  auto joystick = Joystick::findJoystick(iEvent->index);
  if(joystick)
  {
    if(iEvent->connected)
      joystick->connect(iEvent);
    else
      joystick->disconnect(iEvent);
    fPresentJoystickCount = Joystick::computePresentJoystickCount();
    if(fJoystickCallback)
      fJoystickCallback(joystick->fId, joystick->isPresent() ? GLFW_CONNECTED : GLFW_DISCONNECTED);
    return true;
  }
  return false;
}
#endif

//------------------------------------------------------------------------
// Context::findWindow
//------------------------------------------------------------------------
std::shared_ptr<Window> Context::findWindow(GLFWwindow *iWindow) const
{
  // shortcut for the most frequent use-case
  if(fCurrentWindowOpaquePtr == iWindow)
    return fCurrentWindow;

#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  auto iter = std::find_if(fWindows.begin(), fWindows.end(), [iWindow](auto &w) { return w->asOpaquePtr() == iWindow; });
  if(iter != fWindows.end())
    return *iter;
  else
    return nullptr;
#else
  return (fSingleWindow && fSingleWindow->asOpaquePtr() == iWindow) ? fSingleWindow : nullptr;
#endif

}

//------------------------------------------------------------------------
// Context::findFocusedOrSingleWindow
//------------------------------------------------------------------------
std::shared_ptr<Window> Context::findFocusedOrSingleWindow() const
{
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  if(fWindows.size() == 1)
  {
    return fWindows[0];
  }
  else
  {
    std::shared_ptr<Window> hoveredWindow{};
    for(auto &w: fWindows)
    {
      if(w->isFocused())
        return w;
      if(w->isHovered())
        hoveredWindow = w;
    }
    if(hoveredWindow)
      return hoveredWindow;
    else
      return findWindow(fLastKnownFocusedWindow);
  }
#else
  return fSingleWindow;
#endif
}

//------------------------------------------------------------------------
// Context::getWindow
//------------------------------------------------------------------------
std::shared_ptr<Window> Context::getWindow(GLFWwindow *iWindow) const
{
  auto window = findWindow(iWindow);
  if(!window)
    kErrorHandler.logError(GLFW_INVALID_VALUE, "window parameter invalid");
  return window;
}


//------------------------------------------------------------------------
// Context::createWindow
//------------------------------------------------------------------------
GLFWwindow *Context::createWindow(int iWidth, int iHeight, const char* iTitle, GLFWmonitor* iMonitor, GLFWwindow* iShare)
{
  if(iWidth <= 0 || iHeight <= 0)
  {
    if(iMonitor)
      kErrorHandler.logError(GLFW_INVALID_VALUE, "Due to security and restrictions of the browser API, for this platform, "
                                                 "you cannot create a full screen window: create a window with a fixed size, "
                                                 "then from a user event call Module.glfwRequestFullscreen (javascript)");
    else
      kErrorHandler.logError(GLFW_INVALID_VALUE, "Invalid width or height (cannot be <= 0)");
    return nullptr;
  }

#ifdef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  if(fSingleWindow)
  {
    kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Cannot create multiple windows when EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT is defined");
    return nullptr;
  }
#endif

  auto window = std::make_shared<Window>(this, fConfig, fScale, iTitle);

  auto const canvasSelector = std::exchange(fConfig.fCanvasSelector, Config::kDefaultCanvasSelector);

  auto res = emscripten_glfw3_window_init(window->asOpaquePtr(), canvasSelector.c_str());
  if(res != EMSCRIPTEN_RESULT_SUCCESS)
  {
    if(res == EMSCRIPTEN_RESULT_UNKNOWN_TARGET)
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Cannot find canvas element with selector [%s]", canvasSelector.c_str());
    if(res == EMSCRIPTEN_RESULT_INVALID_TARGET)
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Duplicate canvas element with selector [%s]", canvasSelector.c_str());
    return nullptr;
  }
  window->init(iWidth, iHeight);

  if(!window->createGLContext())
    return nullptr;

#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  fWindows.emplace_back(window);
#else
  fSingleWindow = window;
#endif

  if(window->isFocused())
    fLastKnownFocusedWindow = window->asOpaquePtr();

  window->registerEventListeners();

  emscripten_glfw3_window_on_created(window->asOpaquePtr());

  return window->asOpaquePtr();
}

//------------------------------------------------------------------------
// Context::destroyWindow
//------------------------------------------------------------------------
void Context::destroyWindow(GLFWwindow *iWindow)
{
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  auto iter = std::find_if(fWindows.begin(), fWindows.end(), [iWindow](auto &w) { return w->asOpaquePtr() == iWindow; });
  if(iter != fWindows.end())
  {
    auto window = *iter;
    window->destroy();
    if(window == fCurrentWindow)
    {
      fCurrentWindow = nullptr;
      fCurrentWindowOpaquePtr = nullptr;
    }
    if(fLastKnownFocusedWindow == iWindow)
      fLastKnownFocusedWindow = nullptr;
    fWindows.erase(iter);
  }
#else
  if(fSingleWindow && fSingleWindow->asOpaquePtr() == iWindow)
  {
    fSingleWindow->destroy();
    fCurrentWindow = nullptr;
    fCurrentWindowOpaquePtr = nullptr;
    fLastKnownFocusedWindow = nullptr;
    fSingleWindow = nullptr;
  }
#endif

}

//------------------------------------------------------------------------
// Context::setWindowTitle
//------------------------------------------------------------------------
void Context::setWindowTitle(GLFWwindow *iWindow, char const *iTitle)
{
  if(auto window = getWindow(iWindow); window)
  {
    window->setTitle(iTitle);

    // do we need to update the browser title?
    if(window->isFocused() || window == findFocusedOrSingleWindow())
      emscripten_glfw3_context_set_title(iTitle);
  }
}

//------------------------------------------------------------------------
// Context::getWindowTitle
//------------------------------------------------------------------------
char const *Context::getWindowTitle(GLFWwindow *iWindow) const
{
  if(auto window = getWindow(iWindow); window)
  {
    return window->getTitle();
  }
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Context::setNextWindowCanvasSelector
//------------------------------------------------------------------------
void Context::setNextWindowCanvasSelector(char const *iCanvasSelector)
{
  fConfig.fCanvasSelector = iCanvasSelector ? iCanvasSelector : Config::kDefaultCanvasSelector;
}

//------------------------------------------------------------------------
// Context::makeContextCurrent
//------------------------------------------------------------------------
void Context::makeContextCurrent(GLFWwindow *iWindow)
{
  auto window = getWindow(iWindow);
  if(window)
  {
    fCurrentWindow = window;
    fCurrentWindowOpaquePtr = iWindow;
    fCurrentWindow->makeGLContextCurrent();
  }
}

//------------------------------------------------------------------------
// Context::getCurrentContext
//------------------------------------------------------------------------
GLFWwindow *Context::getCurrentContext() const
{
  if(fCurrentWindow)
    return fCurrentWindow->asOpaquePtr();
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Context::setWindowHint
//------------------------------------------------------------------------
void Context::setWindowHint(int iHint, int iValue)
{
  switch(iHint)
  {
    // Gl Context
    case GLFW_CLIENT_API:
      fConfig.fClientAPI = iValue;
      break;

    case GLFW_CONTEXT_VERSION_MAJOR:
      fConfig.fContextVersionMajor = iValue;
      break;

    case GLFW_CONTEXT_VERSION_MINOR:
      fConfig.fContextVersionMinor = iValue;
      break;

      // Window
    case GLFW_SCALE_TO_MONITOR:
      kErrorHandler.logWarning("GLFW_SCALE_TO_MONITOR is deprecated for this platform. Use GLFW_SCALE_FRAMEBUFFER instead.");
      fConfig.fScaleToMonitor = toGlfwBool(iValue);
      break;

    case GLFW_SCALE_FRAMEBUFFER:
      fConfig.fScaleFramebuffer = toGlfwBool(iValue);
      break;

    case GLFW_FOCUS_ON_SHOW:
      fConfig.fFocusOnShow = toGlfwBool(iValue);
      break;

    case GLFW_VISIBLE:
      fConfig.fVisible = toGlfwBool(iValue);
      break;

    case GLFW_FOCUSED:
      fConfig.fFocused = toGlfwBool(iValue);
      break;

    case GLFW_RESIZABLE:
      fConfig.fResizable = toGlfwBool(iValue);
      break;

      // Framebuffer
    case GLFW_ALPHA_BITS:
      fConfig.fAlphaBits = iValue;
      break;

    case GLFW_DEPTH_BITS:
      fConfig.fDepthBits = iValue;
      break;

    case GLFW_STENCIL_BITS:
      fConfig.fStencilBits = iValue;
      break;

    case GLFW_SAMPLES:
      fConfig.fSamples = iValue;
      break;

    default:
      kErrorHandler.logWarning("Hint %d not currently supported on this platform.", iHint);
  }
}

//------------------------------------------------------------------------
// Context::setWindowHint
//------------------------------------------------------------------------
void Context::setWindowHint(int iHint, char const *iValue)
{
  kErrorHandler.logWarning("Hint %d not currently supported on this platform.", iHint);
}

//------------------------------------------------------------------------
// Context::getMonitor
//------------------------------------------------------------------------
std::shared_ptr<Monitor> Context::getMonitor(GLFWmonitor *iMonitor) const
{
  auto monitor = findMonitor(iMonitor);
  if(!monitor)
    kErrorHandler.logError(GLFW_INVALID_VALUE, "monitor parameter invalid");
  return monitor;
}

//------------------------------------------------------------------------
// Context::findWindow
//------------------------------------------------------------------------
std::shared_ptr<Monitor> Context::findMonitor(GLFWmonitor *iMonitor) const
{
  if(fCurrentMonitor->asOpaquePtr() == iMonitor)
    return fCurrentMonitor;
  return nullptr;
}

//------------------------------------------------------------------------
// Context::getMonitors
//------------------------------------------------------------------------
GLFWmonitor **Context::getMonitors(int *oCount)
{
  static std::vector<GLFWmonitor *> kMonitors{fCurrentMonitor->asOpaquePtr()};
  *oCount = static_cast<int>(kMonitors.size());
  return kMonitors.data();
}

//------------------------------------------------------------------------
// Context::getPrimaryMonitor
//------------------------------------------------------------------------
GLFWmonitor *Context::getPrimaryMonitor() const
{
  return fCurrentMonitor->asOpaquePtr();
}

//------------------------------------------------------------------------
// Context::getMonitorPos
//------------------------------------------------------------------------
void Context::getMonitorPos(GLFWmonitor *iMonitor, int *oXPos, int *oYPos) const
{
  auto monitor = getMonitor(iMonitor);
  if(monitor)
  {
    if(oXPos)
      *oXPos = 0;
    if(oYPos)
      *oYPos = 0;
  }
}

//------------------------------------------------------------------------
// Context::getMonitorWorkArea
//------------------------------------------------------------------------
void Context::getMonitorWorkArea(GLFWmonitor *iMonitor, int *oXPos, int *oYPos, int *oWidth, int *oHeight) const
{
  auto monitor = getMonitor(iMonitor);
  if(monitor)
  {
    if(oXPos)
      *oXPos = 0;
    if(oYPos)
      *oYPos = 0;
    int width, height;
    emscripten_get_screen_size(&width, &height);
    if(oWidth)
      *oWidth = width;
    if(oHeight)
      *oHeight = height;
  }
}

//------------------------------------------------------------------------
// Context::getMonitorContentScale
//------------------------------------------------------------------------
void Context::getMonitorContentScale(GLFWmonitor *iMonitor, float *oXScale, float *oYScale) const
{
  auto monitor = getMonitor(iMonitor);
  if(monitor)
  {
    if(oXScale)
      *oXScale = fScale;
    if(oYScale)
      *oYScale = fScale;
  }
}

//------------------------------------------------------------------------
// Context::getMonitorContentScale
//------------------------------------------------------------------------
GLFWmonitor *Context::getMonitor(GLFWwindow *iWindow) const
{
  if(getWindow(iWindow))
    // the best I can do right now due to JavaScript API/limitations
    return fCurrentMonitor->asOpaquePtr();
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Context::createStandardCursor
//------------------------------------------------------------------------
GLFWcursor *Context::createStandardCursor(int iShape)
{
  auto cursor = StandardCursor::findCursor(iShape);
  if(!cursor)
  {
    kErrorHandler.logError(GLFW_INVALID_ENUM, "Invalid cursor shape [%d]", iShape);
    return nullptr;
  }
  return cursor->asOpaquePtr();
}

//------------------------------------------------------------------------
// Context::createCursor
//------------------------------------------------------------------------
GLFWcursor *Context::createCursor(GLFWimage const *iImage, int iXHot, int iYHot)
{
  if(iImage)
  {
    auto cursor = std::make_shared<CustomCursor>(iXHot, iYHot);
    emscripten_glfw3_create_custom_cursor(cursor->asOpaquePtr(), iImage->width, iImage->height, iImage->pixels);
    fCustomCursors.emplace_back(cursor);
    return cursor->asOpaquePtr();
  }
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Context::destroyCursor
//------------------------------------------------------------------------
void Context::destroyCursor(GLFWcursor *iCursor)
{
  if(auto standardCursor = StandardCursor::findCursor(iCursor); standardCursor)
  {
    // do nothing... we don't delete standard cursors
  }
  else
  {
    if(auto customCursor = findCustomCursor(iCursor); customCursor)
    {
      emscripten_glfw3_destroy_custom_cursor(customCursor->asOpaquePtr());
      fCustomCursors.erase(std::remove(fCustomCursors.begin(), fCustomCursors.end(), customCursor),
                           fCustomCursors.end());
    }
    else
      kErrorHandler.logError(GLFW_INVALID_VALUE, "Invalid cursor");
  }
}


//------------------------------------------------------------------------
// Context::findCustomCursor
//------------------------------------------------------------------------
std::shared_ptr<CustomCursor> Context::findCustomCursor(GLFWcursor *iCursor) const
{
  auto i = std::find_if(fCustomCursors.begin(), fCustomCursors.end(), [iCursor](auto &c) { return c->asOpaquePtr() == iCursor; });
  return i == fCustomCursors.end() ? nullptr : *i;
}


//------------------------------------------------------------------------
// Context::setCursor
//------------------------------------------------------------------------
void Context::setCursor(GLFWwindow *iWindow, GLFWcursor *iCursor)
{
  if(auto window = getWindow(iWindow); window)
  {
    if(auto standardCursor = StandardCursor::findCursor(iCursor); standardCursor)
      window->setCursor(standardCursor);
    else
    {
      if(auto customCursor = findCustomCursor(iCursor); customCursor)
        window->setCursor(customCursor);
      else
        kErrorHandler.logError(GLFW_INVALID_VALUE, "Invalid cursor");
    }
  }
}

//------------------------------------------------------------------------
// Context::getTimeInSeconds
//------------------------------------------------------------------------
double Context::getTimeInSeconds() const
{
  return (getPlatformTimerValue() - fInitialTime) / kTimerFrequency;
}

//------------------------------------------------------------------------
// Context::setTimeInSeconds
//------------------------------------------------------------------------
void Context::setTimeInSeconds(double iValue)
{
  if(iValue != iValue || iValue < 0 || iValue >= 18446744073)
  {
    kErrorHandler.logError(GLFW_INVALID_VALUE, "Invalid time [%f]", iValue);
    return;
  }
  fInitialTime = getPlatformTimerValue() - (iValue * kTimerFrequency);
}

//------------------------------------------------------------------------
// Context::getPlatformTimerValue
//------------------------------------------------------------------------
double Context::getPlatformTimerValue()
{
  return emscripten_glfw3_context_get_now();
}

//------------------------------------------------------------------------
// Context::getTimerValue
//------------------------------------------------------------------------
uint64_t Context::getTimerValue()
{
  return static_cast<uint64_t>(getPlatformTimerValue());
}

//------------------------------------------------------------------------
// Context::pollEvents
//------------------------------------------------------------------------
void Context::pollEvents()
{
  computeWindowPos();

#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  for(auto &w: fWindows)
  {
    w->handleSuperPlusKeys(fSuperPlusKeyTimeout);
  }
#else
  if(fSingleWindow)
    fSingleWindow->handleSuperPlusKeys(fSuperPlusKeyTimeout);
#endif

#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
  if(fPresentJoystickCount > 0)
    fPresentJoystickCount = Joystick::pollJoysticks();
#endif
}

//------------------------------------------------------------------------
// Context::swapInterval
//------------------------------------------------------------------------
void Context::swapInterval(int iInterval) const
{
  // Code copied from library_glfw.js
  // GLFW uses negative values to enable GLX_EXT_swap_control_tear, which we don't have,
  // so just treat negative and positive the same.
  iInterval = std::abs(iInterval);
  if(iInterval == 0)
    emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, 0);
  else
    emscripten_set_main_loop_timing(EM_TIMING_RAF, iInterval);
}


//------------------------------------------------------------------------
// Context::glfwExtensionSupported
//------------------------------------------------------------------------
glfw_bool_t Context::isExtensionSupported(char const *extension)
{
  if(extension)
    return toGlfwBool(emscripten_glfw3_context_is_extension_supported(extension));
  else
    return GLFW_FALSE;
}

//------------------------------------------------------------------------
// Context::setClipboardString
//------------------------------------------------------------------------
void Context::setClipboardString(char const *iContent)
{
  fClipboard.setText(iContent);
}

//------------------------------------------------------------------------
// Context::getClipboardString
//------------------------------------------------------------------------
char const *Context::getClipboardString()
{
  auto const &text = fClipboard.getText();
  return text ? text->c_str() : nullptr;
}

//------------------------------------------------------------------------
// Context::openURL
//------------------------------------------------------------------------
void Context::openURL(std::string_view url, std::optional<std::string_view> target)
{
  emscripten_glfw3_context_open_url(url.data(), target ? target->data() : nullptr);
}

//------------------------------------------------------------------------
// Context::isRuntimePlatformApple
//------------------------------------------------------------------------
bool Context::isRuntimePlatformApple() const
{
  return emscripten_glfw3_context_is_runtime_platform_apple();
}

}