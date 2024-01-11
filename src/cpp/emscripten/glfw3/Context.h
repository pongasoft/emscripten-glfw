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
#include "Monitor.h"
#include "Joystick.h"
#include <vector>
#include <string>
#include <optional>

namespace emscripten::glfw3 {

class Context
{
public:
  static std::unique_ptr<Context> init();
  ~Context();

public:
  void defaultWindowHints() { fConfig = {}; }
  void setWindowHint(int iHint, int iValue);
  void setWindowHint(int iHint, char const *iValue);

  GLFWwindow* createWindow(int iWidth, int iHeight, const char* iTitle, GLFWmonitor* iMonitor, GLFWwindow* iShare);
  void destroyWindow(GLFWwindow *iWindow);
  std::shared_ptr<Window> getWindow(GLFWwindow *iWindow) const;

  void makeContextCurrent(GLFWwindow* iWindow);
  GLFWwindow* getCurrentContext() const;

  // monitor
  GLFWmonitor** getMonitors(int* oCount);
  GLFWmonitor* getPrimaryMonitor();
  void getMonitorPos(GLFWmonitor* iMonitor, int* oXPos, int* oYPos);
  void getMonitorWorkArea(GLFWmonitor* iMonitor, int* oXPos, int* oYPos, int* oWidth, int* oHeight);
  std::shared_ptr<Monitor> getMonitor(GLFWmonitor *iMonitor) const;
  GLFWmonitorfun setMonitorCallback(GLFWmonitorfun iCallback) { return std::exchange(fMonitorCallback, iCallback); }

  // cursor
  GLFWcursor *createStandardCursor(int iShape);

  // joystick
  GLFWjoystickfun setJoystickCallback(GLFWjoystickfun iCallback) { return std::exchange(fJoystickCallback, iCallback); }

  // time
  double getTimeInSeconds() const;

  // events
  void pollEvents();

public:
  void onScaleChange();
  void onWindowResize(GLFWwindow *iWindow, int iWidth, int iHeight);
  void requestFullscreen(GLFWwindow *iWindow, bool iLockPointer, bool iResizeCanvas);
  void requestPointerLock(GLFWwindow *iWindow);
  void requestPointerUnlock(GLFWwindow *iWindow, glfw_cursor_mode_t iCursorMode);
  void onFocus(GLFWwindow *iWindow) { fLastKnownFocusedWindow = iWindow; }

private:
  Context();
  std::shared_ptr<Window> findWindow(GLFWwindow *iWindow) const;
  std::shared_ptr<Monitor> findMonitor(GLFWmonitor *iMonitor) const;
  static double getAbsoluteTimeInSeconds();
  void addOrRemoveEventListeners(bool iAdd);
  bool onEnterFullscreen(EmscriptenFullscreenChangeEvent const *iEvent);
  bool onExitFullscreen();
  bool onPointerLock(EmscriptenPointerlockChangeEvent const *iEvent);
  bool onPointerUnlock();
  bool onGamepadConnectionChange(EmscriptenGamepadEvent const *iEvent);
  std::shared_ptr<Window> findFocusedOrSingleWindow() const;

private:
  std::vector<std::shared_ptr<Window>> fWindows{};
  GLFWwindow *fCurrentWindowOpaquePtr{};
  std::shared_ptr<Window> fCurrentWindow{};
  std::shared_ptr<Monitor> fCurrentMonitor{new Monitor{}};
  GLFWwindow *fLastKnownFocusedWindow{};
  Config fConfig{};
  float fScale{1.0f};
  double fInitialTimeInSeconds{getAbsoluteTimeInSeconds()};
  int fPresentJoystickCount{};

  std::optional<Window::FullscreenRequest> fFullscreenRequest{};
  std::optional<Window::PointerLockRequest> fPointerLockRequest{};
  std::optional<Window::PointerUnlockRequest> fPointerUnlockRequest{};

  GLFWmonitorfun fMonitorCallback{};
  GLFWjoystickfun fJoystickCallback{};

  EventListener<EmscriptenMouseEvent> fOnMouseButtonUp{};
  EventListener<EmscriptenKeyboardEvent> fOnKeyDown{};
  EventListener<EmscriptenKeyboardEvent> fOnKeyUp{};
  EventListener<EmscriptenFullscreenChangeEvent> fOnFullscreenChange{};
  EventListener<EmscriptenPointerlockChangeEvent> fOnPointerLockChange{};
  EventListener<void> fOnPointerLockError{};
  EventListener<EmscriptenGamepadEvent> fOnGamepadConnected{};
  EventListener<EmscriptenGamepadEvent> fOnGamepadDisconnected{};
};

}

#endif //EMSCRIPTEN_GLFW_CONTEXT_H