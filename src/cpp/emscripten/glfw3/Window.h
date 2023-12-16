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
#include <emscripten/html5.h>
#include "Object.h"
#include "Config.h"
#include "Events.h"
#include <utility>
#include <functional>

namespace emscripten::glfw3 {

class Window : public Object<GLFWwindow>
{
public:
  inline char const *getCanvasSelector() const { return fConfig.fCanvasSelector.data(); }
  inline bool isHiDPIAware() const { return fConfig.fScaleToMonitor == GLFW_TRUE; }
  inline int getShouldClose() const { return fShouldClose; }
  inline void setShouldClose(int iShouldClose) { fShouldClose = iShouldClose; }

  inline int getWidth() const { return fWidth; }
  inline int getHeight() const { return fHeight; }
  inline void getSize(int* oWidth, int* oHeight) const
  {
    if(oWidth) *oWidth = getWidth();
    if(oHeight) *oHeight = getHeight();
  }
  void setSize(int iWidth, int iHeight);
  inline int getFramebufferWidth() const {  return fFramebufferWidth; }
  inline int getFramebufferHeight() const { return fFramebufferHeight; }
  inline void getFramebufferSize(int* oWidth, int* oHeight) const
  {
    if(oWidth) *oWidth = getFramebufferWidth();
    if(oHeight) *oHeight = getFramebufferHeight();
  }
  inline bool isPointOutside(int x, int y) const { return x < 0 || x > fWidth || y < 0 || y > fHeight; }
  inline bool isPointInside(int x, int y) const { return !isPointOutside(x, y); }

  inline GLFWwindowsizefun setSizeCallback(GLFWwindowsizefun iCallback) { return std::exchange(fSizeCallback, iCallback); }
  inline GLFWframebuffersizefun setFramebufferSizeCallback(GLFWframebuffersizefun iCallback) { return std::exchange(fFramebufferSizeCallback, iCallback); }

  void getContentScale(float* iXScale, float* iYScale) const;
  inline GLFWwindowcontentscalefun setContentScaleCallback(GLFWwindowcontentscalefun iCallback) { return std::exchange(fContentScaleCallback, iCallback); }

  // events
  inline GLFWcursorposfun setCursorPosCallback(GLFWcursorposfun iCallback) { return std::exchange(fCursorPosCallback, iCallback); }
  inline GLFWmousebuttonfun setMouseButtonCallback(GLFWmousebuttonfun iCallback) { return std::exchange(fMouse.fButtonCallback, iCallback); }

  // mouse
  inline void getCursorPos(double *oXPos, double *oYPos) const {
    if(oXPos) {*oXPos = fCursorPosX; }
    if(oYPos) {*oYPos = fCursorPosY; }
  }
  int getMouseButton(int iButton);

  // user pointer
  inline void *getUserPointer() const { return fUserPointer; }
  inline void setUserPointer(void *iPointer) { fUserPointer = iPointer; }

  // monitor scale
  void setMonitorScale(float iScale);

  // OpenGL
  bool createGLContext();
  void makeGLContextCurrent();

  Window(Config iConfig, float iMonitorScale);
  ~Window() override;
  constexpr bool isDestroyed() const { return fDestroyed; }


  friend class Context;

protected:
  void destroy();
  void registerEventListeners() { addOrRemoveEventListeners(true); }
  bool onMouseButtonUp(int iEventType, const EmscriptenMouseEvent *iMouseEvent);

private:
  EventListener<EmscriptenMouseEvent> fOnMouseMove{};
  EventListener<EmscriptenMouseEvent> fOnMouseButtonDown{};

private:
  void createEventListeners();
  void addOrRemoveEventListeners(bool iAdd);
  inline float getScale() const { return isHiDPIAware() ? fMonitorScale : 1.0f; }

private:
  struct Mouse
  {
    int fLastButtonAction{GLFW_RELEASE};
    int fLastButton{-1};
    std::array<int, GLFW_MOUSE_BUTTON_LAST + 1> fButtons{GLFW_RELEASE};
    GLFWmousebuttonfun fButtonCallback{};
  };
private:
  Config fConfig;
  float fMonitorScale;
  bool fDestroyed{};
  int fWidth{};
  int fHeight{};
  int fFramebufferWidth{};
  int fFramebufferHeight{};
  int fShouldClose{}; // GLFW bool
  bool fHasGLContext{};
  double fCursorPosX{};
  double fCursorPosY{};
  Mouse fMouse{};
  void *fUserPointer{};
  GLFWwindowcontentscalefun fContentScaleCallback{};
  GLFWwindowsizefun fSizeCallback{};
  GLFWframebuffersizefun fFramebufferSizeCallback{};
  GLFWcursorposfun fCursorPosCallback{};
};

}

#endif //EMSCRIPTEN_GLFW_WINDOW_H