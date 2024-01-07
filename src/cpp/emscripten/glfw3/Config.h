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

#ifndef EMSCRIPTEN_GLFW_CONFIG_H
#define EMSCRIPTEN_GLFW_CONFIG_H

#include <GLFW/glfw3.h>
#include <string>

namespace emscripten::glfw3 {

using glfw_bool_t = int;

constexpr inline bool toCBool(glfw_bool_t iGlfwBool) { return iGlfwBool != GLFW_FALSE; }
constexpr inline glfw_bool_t toGlfwBool(bool iCBool) { return iCBool ? GLFW_TRUE : GLFW_FALSE; }
constexpr inline glfw_bool_t toGlfwBool(int iValue) { return iValue == GLFW_FALSE ? GLFW_FALSE : GLFW_TRUE; }
constexpr inline char const *boolToString(bool b) { return b ? "true" : "false"; }

struct Config
{
  // For backward compatibility with emscripten, defaults to getting the canvas from Module
  static constexpr char const *kDefaultCanvasSelector = "Module['canvas']";

  // GL Context
  int fClientAPI{GLFW_OPENGL_API}; // GLFW_CLIENT_API

  // Window
  glfw_bool_t fScaleToMonitor{GLFW_FALSE}; // GLFW_SCALE_TO_MONITOR
  glfw_bool_t fFocusOnShow{GLFW_TRUE};     // GLFW_FOCUS_ON_SHOW
  std::string fCanvasSelector{kDefaultCanvasSelector};

  // Framebuffer
  int fAlphaBits   {8};  // GLFW_ALPHA_BITS
  int fDepthBits   {24}; // GLFW_DEPTH_BITS
  int fStencilBits {8};  // GLFW_STENCIL_BITS
  int fSamples     {0};  // GLFW_SAMPLES
};

}

#endif //EMSCRIPTEN_GLFW_CONFIG_H
