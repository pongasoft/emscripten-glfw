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
#include <functional>

extern "C" {
void emscripten_glfw3_context_window_destroy(GLFWwindow *iWindow);
void emscripten_glfw3_context_window_set_size(GLFWwindow *iWindow, int iWidth, int iHeight, int iFramebufferWidth, int iFramebufferHeight);
void emscripten_glfw3_context_gl_init(GLFWwindow *iWindow);
void emscripten_glfw3_context_gl_bool_attribute(GLFWwindow *iWindow, char const *iAttributeName, bool iAttributeValue);
int emscripten_glfw3_context_gl_create_context(GLFWwindow *iWindow);
int emscripten_glfw3_context_gl_make_context_current(GLFWwindow *iWindow);
}

namespace emscripten::glfw3 {

static ErrorHandler &kErrorHandler = ErrorHandler::instance();

//------------------------------------------------------------------------
// Window::Window
//------------------------------------------------------------------------
Window::Window(Config iConfig, float iMonitorScale) : fConfig{std::move(iConfig)}, fMonitorScale{iMonitorScale}
{
  createEventListeners();
}

//------------------------------------------------------------------------
// Window::~Window
//------------------------------------------------------------------------
Window::~Window()
{
  addOrRemoveEventListeners(false);
  emscripten_glfw3_context_window_destroy(asOpaquePtr());
}

//------------------------------------------------------------------------
// Window::setMonitorScale
//------------------------------------------------------------------------
void Window::setMonitorScale(float iScale)
{
  if(fMonitorScale != iScale)
  {
    auto oldScale = getScale();
    fMonitorScale = iScale;
    auto newScale = getScale();
    if(oldScale != newScale && fContentScaleCallback)
      fContentScaleCallback(asOpaquePtr(), fMonitorScale, fMonitorScale);
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
    fbWidth = static_cast<int>(std::floor(static_cast<float>(iWidth) * fMonitorScale));
    fbHeight = static_cast<int>(std::floor(static_cast<float>(iHeight) * fMonitorScale));
  }

  auto framebufferSizeChanged = fFramebufferWidth != fbWidth || fFramebufferHeight != fbHeight;

  fFramebufferWidth = fbWidth;
  fFramebufferHeight = fbHeight;

  emscripten_glfw3_context_window_set_size(asOpaquePtr(), fWidth, fHeight, fFramebufferWidth, fFramebufferHeight);

  if(sizeChanged && fSizeCallback)
    fSizeCallback(asOpaquePtr(), fWidth, fHeight);

  if(framebufferSizeChanged && fFramebufferSizeCallback)
    fFramebufferSizeCallback(asOpaquePtr(), fFramebufferWidth, fFramebufferHeight);
}

//------------------------------------------------------------------------
// Window::createGLContext
//------------------------------------------------------------------------
bool Window::createGLContext()
{
  if(fConfig.fClientAPI != GLFW_NO_API)
  {
    auto id = asOpaquePtr();
    emscripten_glfw3_context_gl_init(id);
    emscripten_glfw3_context_gl_bool_attribute(id, "antialias", fConfig.fSamples > 0);
    emscripten_glfw3_context_gl_bool_attribute(id, "depth", fConfig.fDepthBits > 0);
    emscripten_glfw3_context_gl_bool_attribute(id, "stencil", fConfig.fStencilBits > 0);
    emscripten_glfw3_context_gl_bool_attribute(id, "alpha", fConfig.fAlphaBits > 0);

    if(emscripten_glfw3_context_gl_create_context(id) != EMSCRIPTEN_RESULT_SUCCESS)
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
    emscripten_glfw3_context_gl_make_context_current(asOpaquePtr());
}

//------------------------------------------------------------------------
// Window::getContentScale
//------------------------------------------------------------------------
void Window::getContentScale(float *iXScale, float *iYScale) const
{
  auto scale = getScale();
  *iXScale = scale;
  *iYScale = scale;
}

//------------------------------------------------------------------------
// WindowCallback
//------------------------------------------------------------------------
template<typename E>
EM_BOOL WindowCallback(int iEventType, E const *iEvent, void *iUserData)
{
  auto cb = reinterpret_cast<Window::EventListener<E> *>(iUserData);
  if(std::invoke(*cb, iEventType, iEvent))
    return EM_TRUE;
  else
    return EM_FALSE;
}

//------------------------------------------------------------------------
// Window::createEventListeners
//------------------------------------------------------------------------
void Window::createEventListeners()
{
  fOnMouseMove = [this](int iEventType, const EmscriptenMouseEvent *iMouseEvent) {
    // TODO: handle pointer lock (an emscripten feature, not a glfw3 feature...)

    auto scale = getScale();
    fCursorPosX = static_cast<double>(iMouseEvent->targetX) * scale;
    fCursorPosY = static_cast<double>(iMouseEvent->targetY) * scale;
    if(fCursorPosCallback)
      fCursorPosCallback(asOpaquePtr(), fCursorPosX, fCursorPosY);
    return true;
  };
}

template<typename E>
using EmscriptenEventCallback = EM_BOOL (*)(int, E const *, void *);

template<typename E>
using EmscriptenListenerFunction = EMSCRIPTEN_RESULT (*)(const char *, void *, EM_BOOL, EmscriptenEventCallback<E>, pthread_t);

//------------------------------------------------------------------------
// addOrRemoveListener
//------------------------------------------------------------------------
template<typename E>
void addOrRemoveListener(EmscriptenListenerFunction<E> iListenerFunction, bool iAdd, char const *iTarget, void *iUserData, bool iUseCapture)
{
  auto error = iListenerFunction(iTarget,
                                 iAdd ? iUserData : nullptr,
                                 iUseCapture ? EM_TRUE : EM_FALSE,
                                 iAdd ? WindowCallback<EmscriptenMouseEvent> : nullptr,
                                 EM_CALLBACK_THREAD_CONTEXT_CALLING_THREAD);

  if(error != EMSCRIPTEN_RESULT_SUCCESS)
  {
    kErrorHandler.logError(error, "Error while registering listener for [%s]", iTarget);
  }
}

//------------------------------------------------------------------------
// Window::addOrRemoveEventListeners
//------------------------------------------------------------------------
void Window::addOrRemoveEventListeners(bool iAdd)
{
  auto selector = getCanvasSelector();
  printf("addOrRemoveEventListeners(%s, %s)\n", selector, iAdd ? "true" : "false");

  addOrRemoveListener<EmscriptenMouseEvent>(emscripten_set_mousemove_callback_on_thread, iAdd, selector, &fOnMouseMove, false);
}



}