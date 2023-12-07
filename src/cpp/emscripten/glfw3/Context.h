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
#include "ErrorHandler.h"
#include <vector>
#include <string>

namespace emscripten::glfw3 {

struct Config
{
  // GL Context
  int fClientAPI   {GLFW_OPENGL_API}; // GLFW_CLIENT_API

  // Window
  int fScaleToMonitor{GLFW_FALSE}; // GLFW_SCALE_TO_MONITOR

  // Framebuffer
  int fAlphaBits   {8};  // GLFW_ALPHA_BITS
  int fDepthBits   {24}; // GLFW_DEPTH_BITS
  int fStencilBits {8};  // GLFW_STENCIL_BITS
  int fSamples     {0};  // GLFW_SAMPLES

  std::string fCanvasSelector{"#canvas"};
};

struct Window
{
  int fId{};
  int fWidth{};
  int fHeight{};
  int fShouldClose{}; // GLFW bool
  bool fHasGLContext{};
  Config fConfig{};

  bool isHiDPIAware() const;
  inline char const *getCanvasSelector() const { return fConfig.fCanvasSelector.data(); }
};

class Context
{
public:
  static std::unique_ptr<Context> init();
  ~Context();

public:
  static inline GLFWerrorfun setErrorCallback(GLFWerrorfun iCallback) { return fErrorHandler.setErrorCallback(iCallback); }
  static inline int getError(const char** iDescription) { return fErrorHandler.popError(iDescription); }

  template<typename ... Args>
  static void logError(int iErrorCode, char const *iErrorMessage, Args... args) { fErrorHandler.logError(iErrorCode, iErrorMessage, std::forward<Args>(args)...); }

  template<typename ... Args>
  static void logWarning(char const *iWarningMessage, Args... args) { fErrorHandler.logWarning(iWarningMessage, std::forward<Args>(args)...); }

public:
  void defaultWindowHints() { fConfig = {}; }
  void windowHint(int iHint, int iValue);

  GLFWwindow* createWindow(int iWidth, int iHeight, const char* iTitle, GLFWmonitor* iMonitor, GLFWwindow* iShare);
  void destroyWindow(GLFWwindow *iWindow);
  int windowShouldClose(GLFWwindow* iWindow) const;
  void setWindowShouldClose(GLFWwindow* iWindow, int iValue);
  void makeContextCurrent(GLFWwindow* iWindow);
  GLFWwindow* getCurrentContext() const;

private:
  Context();
  std::shared_ptr<Window> getWindow(GLFWwindow *iWindow) const;

private:
  static ErrorHandler fErrorHandler;

  std::vector<std::shared_ptr<Window>> fWindows{};
  std::shared_ptr<Window> fCurrentWindow{};
  Config fConfig{};
};

}

#endif //EMSCRIPTEN_GLFW_CONTEXT_H