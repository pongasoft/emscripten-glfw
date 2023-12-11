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

#include <utility>
#include "ErrorHandler.h"

extern "C" {
using ScaleChangeCallback = void (*)(void *);
void emscripten_glfw3_context_init(float iScale, ScaleChangeCallback iScaleChangeCallback, void *iUserData);
void emscripten_glfw3_context_destroy();
int emscripten_glfw3_context_window_init(int iCanvasId, char const *iCanvasSelector);
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
// Context::Context
//------------------------------------------------------------------------
Context::Context()
{
  fScale = static_cast<float>(emscripten_get_device_pixel_ratio());
  emscripten_glfw3_context_init(fScale, ContextScaleChangeCallback, this);
  printf("Context::Context %p\n", this);
}

//------------------------------------------------------------------------
// Context::~Context
//------------------------------------------------------------------------
Context::~Context()
{
  emscripten_glfw3_context_destroy();
}

//------------------------------------------------------------------------
// Context::onScaleChange
//------------------------------------------------------------------------
void Context::onScaleChange()
{
  printf("Context::onScaleChange\n");
  fScale = static_cast<float>(emscripten_get_device_pixel_ratio());
  for(auto const &w: fWindows)
  {
    if(w)
      w->setScale(fScale);
  }
}

//------------------------------------------------------------------------
// Context::getWindow
//------------------------------------------------------------------------
std::shared_ptr<Window> Context::getWindow(GLFWwindow *iWindow) const
{
  // TODO I really dislike this as it is NOT safe
  auto window = reinterpret_cast<Window *>(iWindow);

  if(window && window->getId() < fWindows.size())
  {
    auto sWindow = fWindows[window->getId()];
    if(window == sWindow.get())
      return sWindow;
  }
  kErrorHandler.logError(GLFW_INVALID_VALUE, "window parameter invalid");
  return nullptr;
}

//------------------------------------------------------------------------
// Context::createWindow
//------------------------------------------------------------------------
GLFWwindow *Context::createWindow(int iWidth, int iHeight, const char* iTitle, GLFWmonitor* iMonitor, GLFWwindow* iShare)
{
  auto const id = static_cast<int>(fWindows.size());
  auto const canvasSelector = fConfig.fCanvasSelector.data();

  auto res = emscripten_glfw3_context_window_init(id, canvasSelector);
  if(res != EMSCRIPTEN_RESULT_SUCCESS)
  {
    if(res == EMSCRIPTEN_RESULT_UNKNOWN_TARGET)
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Cannot find canvas element with selector [%s]", canvasSelector);
    if(res == EMSCRIPTEN_RESULT_INVALID_TARGET)
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Duplicate canvas element with selector [%s]", canvasSelector);
    return nullptr;
  }

  auto window = std::make_unique<Window>(id, fConfig, fScale);

  window->setSize(iWidth, iHeight);

  if(!window->createGLContext())
    return nullptr;

  fWindows.emplace_back(std::move(window));
  return fWindows[id]->asGLFWwindow();
}

//------------------------------------------------------------------------
// Context::destroyWindow
//------------------------------------------------------------------------
void Context::destroyWindow(GLFWwindow *iWindow)
{
  auto window = getWindow(iWindow);
  if(window)
  {
    if(window == fCurrentWindow)
      fCurrentWindow = nullptr;
    fWindows[window->getId()] = nullptr;
  }
}

//------------------------------------------------------------------------
// Context::windowShouldClose
//------------------------------------------------------------------------
int Context::windowShouldClose(GLFWwindow *iWindow) const
{
  auto window = getWindow(iWindow);
  if(window)
    return window->getShouldClose();
  else
    return GLFW_TRUE;
}

//------------------------------------------------------------------------
// Context::setWindowShouldClose
//------------------------------------------------------------------------
void Context::setWindowShouldClose(GLFWwindow *iWindow, int iValue)
{
  auto window = getWindow(iWindow);
  if(window)
    window->setShouldClose(iValue);
}

//------------------------------------------------------------------------
// Context::makeContextCurrent
//------------------------------------------------------------------------
void Context::makeContextCurrent(GLFWwindow *iWindow)
{
  fCurrentWindow = getWindow(iWindow);
  if(fCurrentWindow)
    fCurrentWindow->makeGLContextCurrent();
}

//------------------------------------------------------------------------
// Context::getCurrentContext
//------------------------------------------------------------------------
GLFWwindow *Context::getCurrentContext() const
{
  if(fCurrentWindow)
    return reinterpret_cast<GLFWwindow *>(fCurrentWindow.get());
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
// Context::setWindowContentScaleCallback
//------------------------------------------------------------------------
GLFWwindowcontentscalefun Context::setWindowContentScaleCallback(GLFWwindow *iWindow,
                                                                 GLFWwindowcontentscalefun iCallback)
{
  auto window = getWindow(iWindow);
  if(window)
    return window->setContentScaleCallback(iCallback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Context::getWindowContentScale
//------------------------------------------------------------------------
void Context::getWindowContentScale(GLFWwindow *iWindow, float *oXScale, float *oYScale)
{
  auto window = getWindow(iWindow);
  if(window)
  {
    window->getContentScale(oXScale, oYScale);
  }
  else
  {
    if(oXScale)
      *oXScale = 1.0f;
    if(oYScale)
      *oYScale = 1.0f;
  }
}

//------------------------------------------------------------------------
// Context::setWindowSize
//------------------------------------------------------------------------
void Context::setWindowSize(GLFWwindow *iWindow, int iWidth, int iHeight)
{
  auto window = getWindow(iWindow);
  if(window)
    window->setSize(iWidth, iHeight);
}

//------------------------------------------------------------------------
// Context::getWindowSize
//------------------------------------------------------------------------
void Context::getWindowSize(GLFWwindow *iWindow, int *oWidth, int *oHeight)
{
  auto window = getWindow(iWindow);
  if(window)
  {
    if(oWidth)
      *oWidth = window->getWidth();
    if(oHeight)
      *oHeight = window->getHeight();
  }
}

//------------------------------------------------------------------------
// Context::getFramebufferSize
//------------------------------------------------------------------------
void Context::getFramebufferSize(GLFWwindow *iWindow, int *oWidth, int *oHeight)
{
  auto window = getWindow(iWindow);
  if(window)
  {
    if(oWidth)
      *oWidth = window->getFramebufferWidth();
    if(oHeight)
      *oHeight = window->getFramebufferHeight();
  }
}

//------------------------------------------------------------------------
// Context::setWindowSizeCallback
//------------------------------------------------------------------------
GLFWwindowsizefun Context::setWindowSizeCallback(GLFWwindow *iWindow, GLFWwindowsizefun iCallback)
{
  auto window = getWindow(iWindow);
  if(window)
    return window->setSizeCallback(iCallback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Context::setFramebufferSizeCallback
//------------------------------------------------------------------------
GLFWframebuffersizefun Context::setFramebufferSizeCallback(GLFWwindow *iWindow, GLFWframebuffersizefun iCallback)
{
  auto window = getWindow(iWindow);
  if(window)
    return window->setFramebufferSizeCallback(iCallback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// Context::getMonitor
//------------------------------------------------------------------------
Monitor *Context::getMonitor(GLFWmonitor *iMonitor) const
{
  // TODO fix this
  auto monitor = reinterpret_cast<Monitor *>(iMonitor);
  if(monitor != &fPrimaryMonitor)
  {
    kErrorHandler.logError(GLFW_INVALID_VALUE, "monitor parameter invalid");
    return nullptr;
  }
  return monitor;
}

//------------------------------------------------------------------------
// Context::getMonitors
//------------------------------------------------------------------------
GLFWmonitor **Context::getMonitors(int *oCount)
{
  static std::vector<GLFWmonitor *> kMonitors{fPrimaryMonitor.asGLFWmonitor()};
  *oCount = static_cast<int>(kMonitors.size());
  return kMonitors.data();
}

//------------------------------------------------------------------------
// Context::getPrimaryMonitor
//------------------------------------------------------------------------
GLFWmonitor *Context::getPrimaryMonitor()
{
  return fPrimaryMonitor.asGLFWmonitor();
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