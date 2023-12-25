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

extern "C" {
using ScaleChangeCallback = void (*)(void *);
using RequestFullscreen = void (*)(void *, GLFWwindow *, bool, bool);
void emscripten_glfw3_context_init(float iScale, ScaleChangeCallback, RequestFullscreen, void *iUserData);
void emscripten_glfw3_context_destroy();
bool emscripten_glfw3_context_is_any_element_focused();
GLFWwindow *emscripten_glfw3_context_get_fullscreen_window();
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
  printf("Detected fullscreen change! %p\n", iUserData);
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
  auto window = iWindow ? getWindow(iWindow) : findFocusedOrSingleWindow();

  if(window)
  {
    fFullscreenRequest = {iWindow, iLockPointer, iResizeCanvas};
    emscripten_request_fullscreen(window->getCanvasSelector(), false);
  }
}

//------------------------------------------------------------------------
// Context::onEnterFullscreen
//------------------------------------------------------------------------
bool Context::onEnterFullscreen(EmscriptenFullscreenChangeEvent const *iEvent)
{
  printf("onEnterFullscreen %s\n", iEvent->id);

  auto fullscreenRequest = std::exchange(fFullscreenRequest, std::nullopt);

  auto window = findWindow(emscripten_glfw3_context_get_fullscreen_window());
  if(!window)
    // fullscreen is not targeting a known window... don't do anything
    return false;

  if(!fullscreenRequest || fullscreenRequest->fWindow != window->asOpaquePtr())
    fullscreenRequest = {window->asOpaquePtr(), false, false};

  window->enterFullscreen(*fullscreenRequest, iEvent->screenWidth, iEvent->screenHeight);

  return true;
}

//------------------------------------------------------------------------
// Context::onExitFullscreen
//------------------------------------------------------------------------
bool Context::onExitFullscreen()
{
  auto iter = std::find_if(fWindows.begin(), fWindows.end(), [](auto &w) { return w->isFullscreen(); });
  if(iter != fWindows.end())
  {
    (*iter)->exitFullscreen();
    return true;
  }
  else
    return false;

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

  return nullptr;
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
  auto window = std::make_shared<Window>(fConfig, fScale);

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



}