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

#ifndef EMSCRIPTEN_GLFW_WINDOW_H
#define EMSCRIPTEN_GLFW_WINDOW_H

#include <GLFW/glfw3.h>
#include "Config.h"
#include <utility>

namespace emscripten::glfw3 {

class Window
{
public:
  inline int getId() const { return fId; }
  inline char const *getCanvasSelector() const { return fConfig.fCanvasSelector.data(); }
  inline bool isHiDPIAware() const { return fConfig.fScaleToMonitor == GLFW_TRUE; }
  inline int getShouldClose() const { return fShouldClose; }
  inline void setShouldClose(int iShouldClose) { fShouldClose = iShouldClose; }

  inline GLFWwindow *asGLFWwindow() { return reinterpret_cast<GLFWwindow *>(this); }
  inline GLFWwindowcontentscalefun setContentScaleCallback(GLFWwindowcontentscalefun iCallback) {
    return std::exchange(fContentScaleCallback, iCallback);
  }
  void setScale(float iScale);
  void setSize(int iWidth, int iHeight);
  bool createGLContext();
  void makeGLContextCurrent();

  Window(int id, Config iConfig, float iScale) : fId{id}, fConfig{std::move(iConfig)}, fScale{iScale} {}
  ~Window();

private:
  int fId;
  Config fConfig;
  float fScale;
  int fWidth{};
  int fHeight{};
  int fFramebufferWidth{};
  int fFramebufferHeight{};
  int fShouldClose{}; // GLFW bool
  bool fHasGLContext{};
  GLFWwindowcontentscalefun fContentScaleCallback{};
};

}

#endif //EMSCRIPTEN_GLFW_WINDOW_H