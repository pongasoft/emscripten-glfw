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
  inline bool isHiDPIAware() const { return fConfig.fScaleFramebuffer == GLFW_TRUE || fConfig.fScaleToMonitor == GLFW_TRUE; }
  inline int getShouldClose() const { return fShouldClose; }
  inline void setShouldClose(int iShouldClose) { fShouldClose = iShouldClose; }

  inline int getWidth() const { return fSize.width; }
  inline int getHeight() const { return fSize.height; }
  inline void getSize(int* oWidth, int* oHeight) const
  {
    if(oWidth) *oWidth = getWidth();
    if(oHeight) *oHeight = getHeight();
  }
  inline void setSize(Vec2<int> const &iSize) { setCanvasSize(iSize); }
  void setSizeLimits(int iMinWidth, int iMinHeight, int iMaxWidth, int iMaxHeight);
  void setAspectRatio(int iNumerator, int iDenominator);
  inline int getFramebufferWidth() const {  return fFramebufferSize.width; }
  inline int getFramebufferHeight() const { return fFramebufferSize.height; }
  inline void getFramebufferSize(int* oWidth, int* oHeight) const
  {
    if(oWidth) *oWidth = getFramebufferWidth();
    if(oHeight) *oHeight = getFramebufferHeight();
  }
  void getPosition(int *oX, int *oY) const;
//  inline bool isPointOutside(int x, int y) const { return x < 0 || x > fWidth || y < 0 || y > fHeight; }
//  inline bool isPointInside(int x, int y) const { return !isPointOutside(x, y); }

  constexpr char const *getTitle() const { return fTitle ? fTitle->c_str() : nullptr; }
  void setTitle(char const *iTitle) { fTitle = iTitle ? std::optional<std::string>(iTitle): std::nullopt; }

  constexpr bool isFocused() const { return fFocused; }
  void focus();

  constexpr bool isHovered() const { return fHovered; }

  constexpr bool isFullscreen() const { return fFullscreen; }

  constexpr bool isPointerLock() const { return fMouse.isPointerLock(); }

  inline GLFWwindowsizefun setSizeCallback(GLFWwindowsizefun iCallback) { return std::exchange(fSizeCallback, iCallback); }
  inline GLFWframebuffersizefun setFramebufferSizeCallback(GLFWframebuffersizefun iCallback) { return std::exchange(fFramebufferSizeCallback, iCallback); }
  inline GLFWwindowfocusfun setFocusCallback(GLFWwindowfocusfun iCallback) { return std::exchange(fFocusCallback, iCallback); }

  void getContentScale(float* iXScale, float* iYScale) const;
  inline GLFWwindowcontentscalefun setContentScaleCallback(GLFWwindowcontentscalefun iCallback) { return std::exchange(fContentScaleCallback, iCallback); }

  // events
  inline GLFWwindowposfun setPosCallback(GLFWwindowposfun iCallback) { return std::exchange(fPosCallback, iCallback); }
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
  glfw_key_state_t getKeyState(glfw_key_t iKey) { return fKeyboard.getKeyState(iKey); }

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

  // visibility
  inline bool isVisible() const { return fVisible; }
  void setVisibility(bool iVisible);

  // window attributes
  int getAttrib(int iAttrib);
  void setAttrib(int iAttrib, int iValue);

  // OpenGL
  bool createGLContext();
  void makeGLContextCurrent();

  // Canvas
  int makeCanvasResizable(std::string_view canvasResizeSelector,
                          std::optional<std::string_view> handleSelector = std::nullopt);
  int unmakeCanvasResizable();

  Window(Context *iContext, Config iConfig, float iMonitorScale, char const *iTitle);
  ~Window() override;
  constexpr bool isDestroyed() const { return fDestroyed; }

  friend class Context;

protected:
  void init(int iWidth, int iHeight);
  void destroy();
  void registerEventListeners() { addOrRemoveEventListeners(true); }
  bool onMouseButtonUp(const EmscriptenMouseEvent *iMouseEvent);
  inline bool onKeyDown(Keyboard::Event const &iEvent, emscripten::glfw3::browser_key_fun_t const &iBrowserKeyCallback) { return fKeyboard.onKeyDown(asOpaquePtr(), iEvent, iBrowserKeyCallback); }
  inline bool onKeyUp(Keyboard::Event const &iEvent, emscripten::glfw3::browser_key_fun_t const &iBrowserKeyCallback) { return fKeyboard.onKeyUp(asOpaquePtr(), iEvent, iBrowserKeyCallback); }
  void resetAllKeys() { fKeyboard.resetAllKeys(asOpaquePtr()); }
  void handleSuperPlusKeys(Keyboard::SuperPlusKeyTimeout const &iTimeout) { if(fKeyboard.hasSuperPlusKeys()) fKeyboard.handleSuperPlusKeys(asOpaquePtr(), iTimeout); }
  bool onFocusChange(bool iFocus);
  void setCursorMode(glfw_cursor_mode_t iCursorMode);
  bool maybeRescale(std::function<void()> const &iAction);
  inline void setResizable(bool iResizable) { fConfig.fResizable = toGlfwBool(iResizable); }
  inline bool isResizable() const { return toCBool(fConfig.fResizable); }
  void setCanvasSize(Vec2<int> const &iSize);
  void resize(Vec2<int> const &iSize);
  Vec2<int> maybeApplySizeConstraints(Vec2<int> const &iSize) const;

private:
  EventListener<EmscriptenMouseEvent> fOnMouseButtonDown{};
  EventListener<EmscriptenMouseEvent> fOnMouseEnter{};
  EventListener<EmscriptenMouseEvent> fOnMouseLeave{};
  EventListener<EmscriptenWheelEvent> fOnMouseWheel{};
  EventListener<EmscriptenFocusEvent> fOnFocusChange{};
  EventListener<EmscriptenFocusEvent> fOnBlurChange{};

private:
  void addOrRemoveEventListeners(bool iAdd);
  inline float getScale() const { return isHiDPIAware() ? fMonitorScale : 1.0f; }
  void setCursorPos(Vec2<double> const &iPos);
  void onGlobalMouseMove(EmscriptenMouseEvent const *iEvent);
  void computePos();

private:
  Context *fContext;
  Config fConfig;
  float fMonitorScale;
  bool fDestroyed{};
  bool fHovered{};
  bool fFocused{};
  double fLastFocusedTime{};
  bool fVisible{true};
  bool fFullscreen{};
  bool fFocusOnMouse{true};
  Vec2<int> fSize{};
  Vec2<int> fPos{};
  Vec2<int> fFramebufferSize{};
  Vec2<int> fMinSize{GLFW_DONT_CARE, GLFW_DONT_CARE};
  Vec2<int> fMaxSize{GLFW_DONT_CARE, GLFW_DONT_CARE};
  int fAspectRatioNumerator{GLFW_DONT_CARE};
  int fAspectRatioDenominator{GLFW_DONT_CARE};
  std::optional<Vec2<int>> fSizeBeforeFullscreen{};
  std::optional<std::string> fTitle{};
  float fOpacity{1.0f};
  int fShouldClose{}; // GLFW bool
  bool fHasGLContext{};
  Mouse fMouse{};
  Keyboard fKeyboard{};
  void *fUserPointer{};
  GLFWwindowcontentscalefun fContentScaleCallback{};
  GLFWwindowposfun fPosCallback{};
  GLFWwindowsizefun fSizeCallback{};
  GLFWwindowfocusfun fFocusCallback{};
  GLFWframebuffersizefun fFramebufferSizeCallback{};
};

}

#endif //EMSCRIPTEN_GLFW_WINDOW_H