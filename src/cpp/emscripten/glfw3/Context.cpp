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

namespace emscripten::glfw3 {

ErrorHandler Context::fErrorHandler{};

constexpr char const *kCanvasSelector = "#canvas";

//------------------------------------------------------------------------
// Context::init
//------------------------------------------------------------------------
std::unique_ptr<Context> Context::init()
{
  return std::unique_ptr<Context>(new Context{});
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
  if(emscripten_set_canvas_element_size(kCanvasSelector, iWidth, iHeight) != EMSCRIPTEN_RESULT_SUCCESS)
  {
    logError(GLFW_PLATFORM_ERROR, "Cannot find canvas element");
    return nullptr;
  }
//  if(emscripten_set_keypress_callback(kCanvasSelector, this, true, key_callback) != EMSCRIPTEN_RESULT_SUCCESS)
//  {
//    logError(GLFW_PLATFORM_ERROR, "Cannot set keypress callback on canvas");
//    return nullptr;
//  }
  window->fWidth = iWidth;
  window->fHeight = iHeight;
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
}

//------------------------------------------------------------------------
// Context::getCurrentContext
//------------------------------------------------------------------------
GLFWwindow *Context::getCurrentContext()
{
  if(fCurrentWindow)
    return reinterpret_cast<GLFWwindow *>(fCurrentWindow.get());
  else
    return nullptr;
}


}