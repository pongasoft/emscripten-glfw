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

#ifndef EMSCRIPTEN_GLFW_CONTEXT_H
#define EMSCRIPTEN_GLFW_CONTEXT_H

#include <memory>
#include <GLFW/glfw3.h>
#include "Window.h"
#include <vector>
#include <string>

namespace emscripten::glfw3 {

class Context
{
public:
  static std::unique_ptr<Context> init();
  ~Context();

public:
  void defaultWindowHints() { fConfig = {}; }
  void windowHint(int iHint, int iValue);

  GLFWwindow* createWindow(int iWidth, int iHeight, const char* iTitle, GLFWmonitor* iMonitor, GLFWwindow* iShare);
  void destroyWindow(GLFWwindow *iWindow);

  int windowShouldClose(GLFWwindow* iWindow) const;
  void setWindowShouldClose(GLFWwindow* iWindow, int iValue);

  void makeContextCurrent(GLFWwindow* iWindow);
  GLFWwindow* getCurrentContext() const;

  void getWindowContentScale(GLFWwindow* iWindow, float* iXScale, float* iYScale);
  GLFWwindowcontentscalefun setWindowContentScaleCallback(GLFWwindow* iWindow, GLFWwindowcontentscalefun iCallback);

  void setWindowSize(GLFWwindow* iWindow, int iWidth, int iHeight);
  void getWindowSize(GLFWwindow* iWindow, int* iWidth, int* iHeight);
  void getFramebufferSize(GLFWwindow* iWindow, int* iWidth, int* iHeight);
  GLFWwindowsizefun setWindowSizeCallback(GLFWwindow *iWindow, GLFWwindowsizefun iCallback);
  GLFWframebuffersizefun setFramebufferSizeCallback(GLFWwindow *iWindow, GLFWframebuffersizefun iCallback);

public:
  void onScaleChange();

private:
  Context();
  std::shared_ptr<Window> getWindow(GLFWwindow *iWindow) const;

private:
  std::vector<std::shared_ptr<Window>> fWindows{};
  std::shared_ptr<Window> fCurrentWindow{};
  Config fConfig{};
  float fScale{1.0f};
};

}

#endif //EMSCRIPTEN_GLFW_CONTEXT_H