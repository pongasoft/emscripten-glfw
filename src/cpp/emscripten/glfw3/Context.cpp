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
#include "Joystick.h"

extern "C" {
using ScaleChangeCallback = void (*)(void *);
using RequestFullscreen = void (*)(void *, GLFWwindow *, bool, bool);
void emscripten_glfw3_context_init(float iScale, ScaleChangeCallback, RequestFullscreen, void *iUserData);
void emscripten_glfw3_context_destroy();
bool emscripten_glfw3_context_is_any_element_focused();
GLFWwindow *emscripten_glfw3_context_get_fullscreen_window();
GLFWwindow *emscripten_glfw3_context_get_pointer_lock_window();
int emscripten_glfw3_context_window_init(GLFWwindow *iWindow, char const *iCanvasSelector);
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
void ContextScaleChangeCallback(void *iUserData)
{
  printf("Detected content scale change! %p\n", iUserData);
  auto context = reinterpret_cast<Context *>(iUserData);
  context->onScaleChange();
}

//------------------------------------------------------------------------
// ContextRequestFullscreen
//------------------------------------------------------------------------
void ContextRequestFullscreen(void *iUserData, GLFWwindow *iWindow, bool iLockPointer, bool iResizeCanvas)
{
  printf("ContextRequestFullscreen(%p, %s, %s)\n", iWindow, boolToString(iLockPointer), boolToString(iResizeCanvas));
  auto context = reinterpret_cast<Context *>(iUserData);
  context->requestFullscreen(iWindow, iLockPointer, iResizeCanvas);
}

//------------------------------------------------------------------------
// Context::Context
//------------------------------------------------------------------------
Context::Context()
{
  printf("Context::Context %p\n", this);
  fScale = static_cast<float>(emscripten_get_device_pixel_ratio());
  emscripten_glfw3_context_init(fScale, ContextScaleChangeCallback, ContextRequestFullscreen, this);

  // fOnMouseUpButton
  fOnMouseButtonUp = [this](int iEventType, const EmscriptenMouseEvent *iEvent) {
    bool handled = false;
    for(auto &w: fWindows)
      handled |= w->onMouseButtonUp(iEvent);
    return handled;
  };

  // fOnKeyDown
  fOnKeyDown = [this](int iEventType, const EmscriptenKeyboardEvent *iEvent) {
    bool handled = false;
    auto w = findFocusedOrSingleWindow();
    if(w && (w->isFocused() || !emscripten_glfw3_context_is_any_element_focused()))
      handled |= w->onKeyDown(iEvent);
    return handled;
  };

  // fOnKeyUp
  fOnKeyUp = [this](int iEventType, const EmscriptenKeyboardEvent *iEvent) {
    bool handled = false;
    for(auto &w: fWindows)
      handled |= w->onKeyUp(iEvent);
    return handled;
  };

  // fOnFullscreenChange
  fOnFullscreenChange = [this](int iEventType, EmscriptenFullscreenChangeEvent const *iEvent) {
    return toCBool(iEvent->isFullscreen) ? onEnterFullscreen(iEvent) : onExitFullscreen();
  };

  // fOnPointerLockChange
  fOnPointerLockChange = [this](int iEventType, EmscriptenPointerlockChangeEvent const *iEvent) {
    return toCBool(iEvent->isActive) ? onPointerLock(iEvent) : onPointerUnlock();
  };

  // fOnPointerLockError
  fOnPointerLockError = [this](int iEventType, void const *iEvent) {
    kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Error while requesting pointerLock (make sure you call this API from a user initiated event, like a mouse click)");
    fPointerLockRequest = std::nullopt;
    return true;
  };

  // fOnFullscreenChange
  fOnFullscreenChange = [this](int iEventType, EmscriptenFullscreenChangeEvent const *iEvent) {
    return toCBool(iEvent->isFullscreen) ? onEnterFullscreen(iEvent) : onExitFullscreen();
  };

  // fOnGamepadConnectionChange
  fOnGamepadConnectionChange = [this](int iEventType, EmscriptenGamepadEvent const *iEvent) {
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
      printf("fPresentJoystickCount = %d\n", fPresentJoystickCount);
      return true;
    }
    return false;
  };

  addOrRemoveEventListeners(true);
}

//------------------------------------------------------------------------
// Context::~Context
//------------------------------------------------------------------------
Context::~Context()
{
  addOrRemoveEventListeners(false);
  emscripten_glfw3_context_destroy();
}

//------------------------------------------------------------------------
// Context::addOrRemoveEventListeners
//------------------------------------------------------------------------
void Context::addOrRemoveEventListeners(bool iAdd)
{
  printf("Context::addOrRemoveEventListeners(%s)\n", iAdd ? "true" : "false");
  // mouse
  addOrRemoveListener<EmscriptenMouseEvent>(emscripten_set_mouseup_callback_on_thread, iAdd, EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &fOnMouseButtonUp, false);

  // keyboard
  addOrRemoveListener<EmscriptenKeyboardEvent>(emscripten_set_keydown_callback_on_thread, iAdd, EMSCRIPTEN_EVENT_TARGET_WINDOW, &fOnKeyDown, false);
  addOrRemoveListener<EmscriptenKeyboardEvent>(emscripten_set_keyup_callback_on_thread, iAdd, EMSCRIPTEN_EVENT_TARGET_WINDOW, &fOnKeyUp, false);

  // fullscreen
  addOrRemoveListener<EmscriptenFullscreenChangeEvent>(emscripten_set_fullscreenchange_callback_on_thread, iAdd, EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &fOnFullscreenChange, false);

  // pointerLock
  addOrRemoveListener<EmscriptenPointerlockChangeEvent>(emscripten_set_pointerlockchange_callback_on_thread, iAdd, EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &fOnPointerLockChange, false);
  addOrRemoveListener<void>(emscripten_set_pointerlockerror_callback_on_thread, iAdd, EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &fOnPointerLockError, false);

  // gamepad
#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
  addOrRemoveListener2<EmscriptenGamepadEvent>(emscripten_set_gamepadconnected_callback_on_thread, iAdd, &fOnGamepadConnectionChange, false);
  addOrRemoveListener2<EmscriptenGamepadEvent>(emscripten_set_gamepaddisconnected_callback_on_thread, iAdd, &fOnGamepadConnectionChange, false);
#endif

}

//------------------------------------------------------------------------
// Context::onScaleChange
//------------------------------------------------------------------------
void Context::onScaleChange()
{
  printf("Context::onScaleChange\n");
  fScale = static_cast<float>(emscripten_get_device_pixel_ratio());
  for(auto &w: fWindows)
  {
    w->setMonitorScale(fScale);
  }
}

//------------------------------------------------------------------------
// Context::requestFullscreen
//------------------------------------------------------------------------
void Context::requestFullscreen(GLFWwindow *iWindow, bool iLockPointer, bool iResizeCanvas)
{
  // Per spec: If calling requestPointerLock() with requestFullscreen(), the requestPointerLock() must be
  // called first, because the requestFullscreen() will consume the state of transient activation.
  if(iLockPointer)
    requestPointerLock(iWindow);

  auto window = iWindow ? getWindow(iWindow) : findFocusedOrSingleWindow();

  if(window)
  {
    fFullscreenRequest = {window->asOpaquePtr(), iResizeCanvas};
    if(emscripten_request_fullscreen(window->getCanvasSelector(), false) != EMSCRIPTEN_RESULT_SUCCESS)
    {
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Error while requesting fullscreen (make sure you call this API from a user initiated event, like a mouse click)");
      fFullscreenRequest = std::nullopt;
    }
  }
}

//------------------------------------------------------------------------
// Context::requestPointerLock
//------------------------------------------------------------------------
void Context::requestPointerLock(GLFWwindow *iWindow)
{
  auto window = iWindow ? getWindow(iWindow) : findFocusedOrSingleWindow();

  if(window)
  {
    fPointerLockRequest = {iWindow};
    if(emscripten_request_pointerlock(window->getCanvasSelector(), false) != EMSCRIPTEN_RESULT_SUCCESS)
    {
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Error while requesting pointerLock (make sure you call this API from a user initiated event, like a mouse click)");
      fPointerLockRequest = std::nullopt;
    }
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
  printf("onEnterFullscreen %s\n", iEvent->id);

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
  printf("onExitFullscreen\n");

  bool res = false;

  // only 1 window should be in fullscreen
  for(auto &w: fWindows)
    res |= w->onExitFullscreen();

  return res;
}

//------------------------------------------------------------------------
// Context::onPointerLock
//------------------------------------------------------------------------
bool Context::onPointerLock(EmscriptenPointerlockChangeEvent const *iEvent)
{
  printf("onPointerLock %s\n", iEvent->id);

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
  printf("onPointerUnlock\n");

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
  for(auto &w: fWindows)
    res |= w->onPointerUnlock(std::nullopt);

  return res;
}

//------------------------------------------------------------------------
// Context::findWindow
//------------------------------------------------------------------------
std::shared_ptr<Window> Context::findWindow(GLFWwindow *iWindow) const
{
  // shortcut for most frequent use-case
  if(fCurrentWindowOpaquePtr == iWindow)
    return fCurrentWindow;

  auto iter = std::find_if(fWindows.begin(), fWindows.end(), [iWindow](auto &w) { return w->asOpaquePtr() == iWindow; });
  if(iter != fWindows.end())
    return *iter;
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Context::findFocusedOrSingleWindow
//------------------------------------------------------------------------
std::shared_ptr<Window> Context::findFocusedOrSingleWindow() const
{
  if(fWindows.size() == 1)
  {
    return fWindows[0];
  }
  else
  {
    for(auto &w: fWindows)
    {
      if(w->isFocused())
        return w;
    }
  }

  return findWindow(fLastKnownFocusedWindow);
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
  auto window = std::make_shared<Window>(this, fConfig, fScale);

  auto const canvasSelector = fConfig.fCanvasSelector.data();

  auto res = emscripten_glfw3_context_window_init(window->asOpaquePtr(), canvasSelector);
  if(res != EMSCRIPTEN_RESULT_SUCCESS)
  {
    if(res == EMSCRIPTEN_RESULT_UNKNOWN_TARGET)
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Cannot find canvas element with selector [%s]", canvasSelector);
    if(res == EMSCRIPTEN_RESULT_INVALID_TARGET)
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Duplicate canvas element with selector [%s]", canvasSelector);
    return nullptr;
  }
  window->init();
  window->setSize(iWidth, iHeight);

  if(!window->createGLContext())
    return nullptr;

  fWindows.emplace_back(window);

  window->registerEventListeners();

  return window->asOpaquePtr();
}

//------------------------------------------------------------------------
// Context::destroyWindow
//------------------------------------------------------------------------
void Context::destroyWindow(GLFWwindow *iWindow)
{
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
// Context::windowHint
//------------------------------------------------------------------------
void Context::windowHint(int iHint, int iValue)
{
  switch(iHint)
  {
    // Gl Context
    case GLFW_CLIENT_API:
      fConfig.fClientAPI = iValue;
      break;

    // Window
    case GLFW_SCALE_TO_MONITOR:
      fConfig.fScaleToMonitor = iValue;
      break;

    case GLFW_FOCUS_ON_SHOW:
      fConfig.fFocusOnShow = iValue;
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

// Making up a hint that is not currently used: TODO how to add to glfw???
#define GLFW_EMSCRIPTEN_CANVAS_SELECTOR  0x00027001

//------------------------------------------------------------------------
// Context::windowHint
//------------------------------------------------------------------------
void Context::windowHint(int iHint, char const *iValue)
{
  switch(iHint)
  {
    // Gl Context
    case GLFW_EMSCRIPTEN_CANVAS_SELECTOR:
      fConfig.fCanvasSelector = iValue ? iValue : Config::kDefaultCanvasSelector;
      break;

    default:
      kErrorHandler.logWarning("Hint %d not currently supported on this platform.", iHint);
  }

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
GLFWmonitor *Context::getPrimaryMonitor()
{
  return fCurrentMonitor->asOpaquePtr();
}

//------------------------------------------------------------------------
// Context::getMonitorPos
//------------------------------------------------------------------------
void Context::getMonitorPos(GLFWmonitor *iMonitor, int *oXPos, int *oYPos)
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
void Context::getMonitorWorkArea(GLFWmonitor *iMonitor, int *oXPos, int *oYPos, int *oWidth, int *oHeight)
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
// Context::createStandardCursor
//------------------------------------------------------------------------
GLFWcursor *Context::createStandardCursor(int iShape)
{
  auto const *cursor = Cursor::findCursor(iShape);
  if(!cursor)
  {
    kErrorHandler.logError(GLFW_INVALID_ENUM, "Invalid cursor shape [%d]", iShape);
    return nullptr;
  }
  return cursor->asOpaquePtr();
}

//------------------------------------------------------------------------
// Context::getTime
//------------------------------------------------------------------------
double Context::getAbsoluteTimeInSeconds()
{
  return emscripten_get_now() / 1000;
}

//------------------------------------------------------------------------
// Context::getTimeInSeconds
//------------------------------------------------------------------------
double Context::getTimeInSeconds() const
{
  return getAbsoluteTimeInSeconds() - fInitialTimeInSeconds;
}

//------------------------------------------------------------------------
// Context::pollEvents
//------------------------------------------------------------------------
void Context::pollEvents()
{
#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
  if(fPresentJoystickCount > 0)
    fPresentJoystickCount = Joystick::pollJoysticks();
#endif
}

}