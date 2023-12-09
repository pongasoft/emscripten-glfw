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

#include "Window.h"
#include <emscripten/em_types.h>
#include <utility>
#include "ErrorHandler.h"

extern "C" {
void emscripten_glfw3_context_window_destroy(int iCanvasId);
void emscripten_glfw3_context_window_set_size(int iCanvasId, int iWidth, int iHeight, int iFramebufferWidth, int iFramebufferHeight);
void emscripten_glfw3_context_gl_init(int iCanvasId);
void emscripten_glfw3_context_gl_bool_attribute(int iCanvasId, char const *iAttributeName, bool iAttributeValue);
int emscripten_glfw3_context_gl_create_context(int iCanvasId);
int emscripten_glfw3_context_gl_make_context_current(int iCanvasId);
}

namespace emscripten::glfw3 {

static ErrorHandler &kErrorHandler = ErrorHandler::instance();

//------------------------------------------------------------------------
// Window::~Window
//------------------------------------------------------------------------
Window::~Window()
{
  emscripten_glfw3_context_window_destroy(fId);
}

//------------------------------------------------------------------------
// Window::setScale
//------------------------------------------------------------------------
void Window::setScale(float iScale)
{
  if(fScale != iScale)
  {
    fScale = iScale;
    if(fContentScaleCallback)
      fContentScaleCallback(asGLFWwindow(), fScale, fScale);
  }
}

//------------------------------------------------------------------------
// Window::setSize
//------------------------------------------------------------------------
void Window::setSize(int iWidth, int iHeight)
{
  auto sizeChanged = fWidth != iWidth || fHeight != iHeight;
  fWidth = iWidth;
  fHeight = iHeight;

  int fbWidth = iWidth;
  int fbHeight = iHeight;
  
  if(isHiDPIAware())
  {
    fbWidth = static_cast<int>(std::floor(static_cast<float>(iWidth) * fScale));
    fbHeight = static_cast<int>(std::floor(static_cast<float>(iHeight) * fScale));
  }

  auto framebufferSizeChanged = fFramebufferWidth != fbWidth || fFramebufferHeight != fbHeight;

  fFramebufferWidth = fbWidth;
  fFramebufferHeight = fbHeight;

  emscripten_glfw3_context_window_set_size(fId, fWidth, fHeight, fFramebufferWidth, fFramebufferHeight);

  if(sizeChanged && fSizeCallback)
    fSizeCallback(asGLFWwindow(), fWidth, fHeight);

  if(framebufferSizeChanged && fFramebufferSizeCallback)
    fFramebufferSizeCallback(asGLFWwindow(), fFramebufferWidth, fFramebufferHeight);
}

//------------------------------------------------------------------------
// Window::createGLContext
//------------------------------------------------------------------------
bool Window::createGLContext()
{
  if(fConfig.fClientAPI != GLFW_NO_API)
  {
    emscripten_glfw3_context_gl_init(fId);
    emscripten_glfw3_context_gl_bool_attribute(fId, "antialias", fConfig.fSamples > 0);
    emscripten_glfw3_context_gl_bool_attribute(fId, "depth", fConfig.fDepthBits > 0);
    emscripten_glfw3_context_gl_bool_attribute(fId, "stencil", fConfig.fStencilBits > 0);
    emscripten_glfw3_context_gl_bool_attribute(fId, "alpha", fConfig.fAlphaBits > 0);

    if(emscripten_glfw3_context_gl_create_context(fId) != EMSCRIPTEN_RESULT_SUCCESS)
    {
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Cannot create GL context for [%s]", getCanvasSelector());
      return false;
    }

    fHasGLContext = true;
  }

  return true;
}

//------------------------------------------------------------------------
// Window::makeGLContextCurrent
//------------------------------------------------------------------------
void Window::makeGLContextCurrent()
{
  if(fHasGLContext)
    emscripten_glfw3_context_gl_make_context_current(fId);
}

//------------------------------------------------------------------------
// Window::getContentScale
//------------------------------------------------------------------------
void Window::getContentScale(float *iXScale, float *iYScale) const
{
  auto scale = isHiDPIAware() ? fScale : 1.0f;
  *iXScale = scale;
  *iYScale = scale;
}

}