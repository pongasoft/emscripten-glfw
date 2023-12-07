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

extern "C" {
void emscripten_glfw3_context_init();
void emscripten_glfw3_context_destroy();
int emscripten_glfw3_context_gl_init(int iCanvasId, char const *iCanvasSelector);
int emscripten_glfw3_context_gl_bool_attribute(int iCanvasId, char const *iAttributeName, bool iAttributeValue);
int emscripten_glfw3_context_gl_create_context(int iCanvasId);
int emscripten_glfw3_context_gl_make_context_current(int iCanvasId);
}

namespace emscripten::glfw3 {

ErrorHandler Context::fErrorHandler{};

//------------------------------------------------------------------------
// Context::init
//------------------------------------------------------------------------
std::unique_ptr<Context> Context::init()
{
  return std::unique_ptr<Context>(new Context{});
}

//------------------------------------------------------------------------
// Context::Context
//------------------------------------------------------------------------
Context::Context()
{
  emscripten_glfw3_context_init();
}

//------------------------------------------------------------------------
// Context::~Context
//------------------------------------------------------------------------
Context::~Context()
{
  emscripten_glfw3_context_destroy();
}

//------------------------------------------------------------------------
// Context::getWindow
//------------------------------------------------------------------------
std::shared_ptr<Window> Context::getWindow(GLFWwindow *iWindow) const
{
  auto window = reinterpret_cast<Window *>(iWindow);

  if(window && window->fId < fWindows.size())
  {
    auto sWindow = fWindows[window->fId];
    if(window == sWindow.get())
      return sWindow;
  }
  logError(GLFW_INVALID_VALUE, "window parameter invalid");
  return nullptr;
}

//------------------------------------------------------------------------
// Context::createWindow
//------------------------------------------------------------------------
GLFWwindow *Context::createWindow(int iWidth, int iHeight, const char* iTitle, GLFWmonitor* iMonitor, GLFWwindow* iShare)
{
  auto window = std::make_unique<Window>();
  auto id = static_cast<int>(fWindows.size());
  window->fId = id;
  window->fConfig = fConfig;
  if(emscripten_set_canvas_element_size(window->getCanvasSelector(), iWidth, iHeight) != EMSCRIPTEN_RESULT_SUCCESS)
  {
    logError(GLFW_PLATFORM_ERROR, "Cannot find canvas element with selector [%s]", window->getCanvasSelector());
    return nullptr;
  }
//  if(emscripten_set_keypress_callback(kCanvasSelector, this, true, key_callback) != EMSCRIPTEN_RESULT_SUCCESS)
//  {
//    logError(GLFW_PLATFORM_ERROR, "Cannot set keypress callback on canvas");
//    return nullptr;
//  }
  window->fWidth = iWidth;
  window->fHeight = iHeight;

  // if hint GLFW_CLIENT_API != GLFW_NO_API

  if(window->fConfig.fClientAPI != GLFW_NO_API)
  {
    if(emscripten_glfw3_context_gl_init(window->fId, window->getCanvasSelector()) == EMSCRIPTEN_RESULT_SUCCESS)
    {
      emscripten_glfw3_context_gl_bool_attribute(window->fId, "antialias", window->fConfig.fSamples > 0);
      emscripten_glfw3_context_gl_bool_attribute(window->fId, "depth", window->fConfig.fDepthBits > 0);
      emscripten_glfw3_context_gl_bool_attribute(window->fId, "stencil", window->fConfig.fStencilBits > 0);
      emscripten_glfw3_context_gl_bool_attribute(window->fId, "alpha", window->fConfig.fAlphaBits > 0);

      emscripten_glfw3_context_gl_create_context(window->fId);

      window->fHasGLContext = true;
    }
  }


  /*
   *           var contextAttributes = {
            antialias: (GLFW.hints[0x0002100D] > 1), // GLFW_SAMPLES
            depth: (GLFW.hints[0x00021005] > 0),     // GLFW_DEPTH_BITS
            stencil: (GLFW.hints[0x00021006] > 0),   // GLFW_STENCIL_BITS
            alpha: (GLFW.hints[0x00021004] > 0)      // GLFW_ALPHA_BITS
          }

   */

  fWindows.emplace_back(std::move(window));
  return reinterpret_cast<GLFWwindow *>(fWindows[id].get());
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
    fWindows[window->fId] = nullptr;
  }
}

//------------------------------------------------------------------------
// Context::windowShouldClose
//------------------------------------------------------------------------
int Context::windowShouldClose(GLFWwindow *iWindow) const
{
  auto window = getWindow(iWindow);
  if(window)
    return window->fShouldClose;
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
    window->fShouldClose = iValue;
}

//------------------------------------------------------------------------
// Context::makeContextCurrent
//------------------------------------------------------------------------
void Context::makeContextCurrent(GLFWwindow *iWindow)
{
  fCurrentWindow = getWindow(iWindow);
  if(fCurrentWindow && fCurrentWindow->fHasGLContext)
    emscripten_glfw3_context_gl_make_context_current(fCurrentWindow->fId);
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
    case GLFW_OPENGL_API:
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
      logWarning("Hint %d not currently supported on this platform.", iHint);
  }

}

//------------------------------------------------------------------------
// Window::isHiDPIAware
//------------------------------------------------------------------------
bool Window::isHiDPIAware() const
{
  return fConfig.fScaleToMonitor == GLFW_TRUE;
}

}