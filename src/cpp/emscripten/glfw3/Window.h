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
#include "Mouse.h"
#include "Keyboard.h"
#include "Types.h"
#include "Cursor.h"
#include <utility>
#include <optional>
#include <functional>

namespace emscripten::glfw3 {

class Context;

class Window : public Object<GLFWwindow>
{
public:
  struct FullscreenRequest
  {
    GLFWwindow *fWindow{};
    bool fResizeCanvas{};
  };

  struct PointerLockRequest
  {
    GLFWwindow *fWindow{};
  };

  struct PointerUnlockRequest
  {
    GLFWwindow *fWindow{};
    glfw_cursor_mode_t fCursorMode;
  };

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
//  inline bool isPointOutside(int x, int y) const { return x < 0 || x > fWidth || y < 0 || y > fHeight; }
//  inline bool isPointInside(int x, int y) const { return !isPointOutside(x, y); }

  constexpr bool isFocused() const { return fFocused; }
  void focus();

  constexpr bool isFullscreen() const { return fFullscreen; }

  constexpr bool isPointerLock() const { return fMouse.isPointerLock(); }

  inline GLFWwindowsizefun setSizeCallback(GLFWwindowsizefun iCallback) { return std::exchange(fSizeCallback, iCallback); }
  inline GLFWframebuffersizefun setFramebufferSizeCallback(GLFWframebuffersizefun iCallback) { return std::exchange(fFramebufferSizeCallback, iCallback); }
  inline GLFWwindowfocusfun setFocusCallback(GLFWwindowfocusfun iCallback) { return std::exchange(fFocusCallback, iCallback); }

  void getContentScale(float* iXScale, float* iYScale) const;
  inline GLFWwindowcontentscalefun setContentScaleCallback(GLFWwindowcontentscalefun iCallback) { return std::exchange(fContentScaleCallback, iCallback); }

  // events
  inline GLFWcursorposfun setCursorPosCallback(GLFWcursorposfun iCallback) { return std::exchange(fMouse.fCursorPosCallback, iCallback); }
  inline GLFWmousebuttonfun setMouseButtonCallback(GLFWmousebuttonfun iCallback) { return std::exchange(fMouse.fButtonCallback, iCallback); }
  inline GLFWscrollfun setScrollCallback(GLFWscrollfun iCallback) { return std::exchange(fMouse.fScrollCallback, iCallback); }
  inline GLFWcursorenterfun setCursorEnterCallback(GLFWcursorenterfun iCallback) { return std::exchange(fMouse.fCursorEnterCallback, iCallback); }
  inline GLFWkeyfun setKeyCallback(GLFWkeyfun iCallback) { return fKeyboard.setKeyCallback(iCallback); }
  inline GLFWcharfun setCharCallback(GLFWcharfun iCallback) { return fKeyboard.setCharCallback(iCallback); }

  // mouse
  inline void getCursorPos(double *oXPos, double *oYPos) const {
    if(oXPos) {*oXPos = fMouse.fCursorPos.x; }
    if(oYPos) {*oYPos = fMouse.fCursorPos.y; }
  }
  glfw_mouse_button_state_t getMouseButtonState(glfw_mouse_button_t iButton);
  void setCursor(GLFWcursor* iCursor);

  // keyboard
  glfw_key_state_t getKeyState(glfw_key_t iKey) const { return fKeyboard.getKeyState(iKey); }

  // user pointer
  inline void *getUserPointer() const { return fUserPointer; }
  inline void setUserPointer(void *iPointer) { fUserPointer = iPointer; }

  // input mode
  void setInputMode(int iMode, int iValue);
  int getInputMode(int iMode) const;

  // monitor scale
  void setMonitorScale(float iScale);

  // fullscreen
  void onEnterFullscreen(std::optional<Vec2<int>> const &iScreenSize);
  bool onExitFullscreen();

  // pointerLock
  void onPointerLock();
  bool onPointerUnlock(std::optional<glfw_cursor_mode_t> iCursorMode);

  // opacity
  float getOpacity() const { return fOpacity; }
  void setOpacity(float iOpacity);

  // OpenGL
  bool createGLContext();
  void makeGLContextCurrent();

  Window(Context *iContext, Config iConfig, float iMonitorScale);
  ~Window() override;
  constexpr bool isDestroyed() const { return fDestroyed; }

  friend class Context;

protected:
  void destroy();
  void registerEventListeners() { addOrRemoveEventListeners(true); }
  bool onMouseButtonUp(const EmscriptenMouseEvent *iMouseEvent);
  inline bool onKeyDown(const EmscriptenKeyboardEvent *iKeyboardEvent) { return fKeyboard.onKeyDown(asOpaquePtr(), iKeyboardEvent); }
  inline bool onKeyUp(const EmscriptenKeyboardEvent *iKeyboardEvent) { return fKeyboard.onKeyUp(asOpaquePtr(), iKeyboardEvent); }
  void setCursorMode(glfw_cursor_mode_t iCursorMode);

private:
  EventListener<EmscriptenMouseEvent> fOnMouseMove{};
  EventListener<EmscriptenMouseEvent> fOnMouseButtonDown{};
  EventListener<EmscriptenMouseEvent> fOnMouseEnterOrLeave{};
  EventListener<EmscriptenWheelEvent> fOnMouseWheel{};
  EventListener<EmscriptenFocusEvent> fOnFocusChange{};

private:
  void createEventListeners();
  void addOrRemoveEventListeners(bool iAdd);
  inline float getScale() const { return isHiDPIAware() ? fMonitorScale : 1.0f; }
  void setCursorPos(Vec2<double> const &iPos);

private:
  Context *fContext;
  Config fConfig;
  float fMonitorScale;
  bool fDestroyed{};
  bool fFocused{};
  bool fFullscreen{};
  bool fFocusOnMouse{true};
  int fWidth{};
  int fHeight{};
  int fFramebufferWidth{};
  int fFramebufferHeight{};
  std::optional<int> fWidthBeforeFullscreen{};
  std::optional<int> fHeightBeforeFullscreen{};
  float fOpacity{1.0f};
  int fShouldClose{}; // GLFW bool
  bool fHasGLContext{};
  Mouse fMouse{};
  Keyboard fKeyboard{};
  void *fUserPointer{};
  GLFWwindowcontentscalefun fContentScaleCallback{};
  GLFWwindowsizefun fSizeCallback{};
  GLFWwindowfocusfun fFocusCallback{};
  GLFWframebuffersizefun fFramebufferSizeCallback{};
};

}

#endif //EMSCRIPTEN_GLFW_WINDOW_H