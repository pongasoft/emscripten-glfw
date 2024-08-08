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
#include <string>
#include <optional>
#include <future>
#include <vector>
#include <GLFW/emscripten_glfw3.h>

#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
#include "Joystick.h"
#endif

#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
#include <vector>
#endif

namespace emscripten::glfw3 {

// in javascript the performance/DOMHighResTimeStamp measures in milliseconds
constexpr static uint64_t kTimerFrequency = 1000;

class Context
{
public:
  static std::unique_ptr<Context> init();
  ~Context();

public:
  void terminate();
  void defaultWindowHints() { fConfig = {}; }
  void setWindowHint(int iHint, int iValue);
  void setWindowHint(int iHint, char const *iValue);

  // window
  GLFWwindow* createWindow(int iWidth, int iHeight, const char* iTitle, GLFWmonitor* iMonitor, GLFWwindow* iShare);
  void destroyWindow(GLFWwindow *iWindow);
  std::shared_ptr<Window> getWindow(GLFWwindow *iWindow) const;
  void setWindowTitle(GLFWwindow *iWindow, char const *iTitle);
  char const *getWindowTitle(GLFWwindow *iWindow) const;
  void setNextWindowCanvasSelector(char const *iCanvasSelector);

  void makeContextCurrent(GLFWwindow* iWindow);
  GLFWwindow* getCurrentContext() const;

  // monitor
  GLFWmonitor** getMonitors(int* oCount);
  GLFWmonitor* getPrimaryMonitor() const;
  void getMonitorPos(GLFWmonitor* iMonitor, int* oXPos, int* oYPos) const;
  void getMonitorWorkArea(GLFWmonitor* iMonitor, int* oXPos, int* oYPos, int* oWidth, int* oHeight) const;
  void getMonitorContentScale(GLFWmonitor* iMonitor, float *oXScale, float* oYScale) const;
  std::shared_ptr<Monitor> getMonitor(GLFWmonitor *iMonitor) const;
  GLFWmonitorfun setMonitorCallback(GLFWmonitorfun iCallback) { return std::exchange(fMonitorCallback, iCallback); }
  GLFWmonitor *getMonitor(GLFWwindow *iWindow) const;

  // cursor
  GLFWcursor *createStandardCursor(int iShape);

  // joystick
  GLFWjoystickfun setJoystickCallback(GLFWjoystickfun iCallback) { return std::exchange(fJoystickCallback, iCallback); }

  // time
  double getTimeInSeconds() const;
  void setTimeInSeconds(double iValue);
  static uint64_t getTimerValue() ;

  // events
  void pollEvents();
  void swapInterval(int iInterval) const;

  // opengl
  glfw_bool_t isExtensionSupported(const char* extension);

  // clipboard
  void setClipboardString(char const *iContent);
  char const *getClipboardString();
  std::future<ClipboardString> asyncGetClipboardString();
  void getClipboardString(emscripten_glfw_clipboard_string_fun iCallback, void *iUserData = nullptr);

  // keyboard
  int getSuperPlusKeyTimeout() const { return fSuperPlusKeyTimeout; }
  void setSuperPlusKeyTimeout(int iTimeoutMilliseconds) { fSuperPlusKeyTimeout = iTimeoutMilliseconds; }


  // misc
  void openURL(std::string_view url, std::optional<std::string_view> target);

public:
  void onScaleChange();
  void onWindowResize(GLFWwindow *iWindow, int iWidth, int iHeight);
  void onClipboardString(char const *iText, char const *iErrorMessage);
  int requestFullscreen(GLFWwindow *iWindow, bool iLockPointer, bool iResizeCanvas);
  int requestPointerLock(GLFWwindow *iWindow);
  void requestPointerUnlock(GLFWwindow *iWindow, glfw_cursor_mode_t iCursorMode);
  void onFocus(GLFWwindow *iWindow) { fLastKnownFocusedWindow = iWindow; }
  bool onKeyDown(Keyboard::Event const &iEvent);
  bool onKeyUp(Keyboard::Event const &iEvent);

private:
  Context();
  std::shared_ptr<Window> findWindow(GLFWwindow *iWindow) const;
  std::shared_ptr<Monitor> findMonitor(GLFWmonitor *iMonitor) const;
  void addOrRemoveEventListeners(bool iAdd);
  bool onEnterFullscreen(EmscriptenFullscreenChangeEvent const *iEvent);
  bool onExitFullscreen();
  bool onPointerLock(EmscriptenPointerlockChangeEvent const *iEvent);
  bool onPointerUnlock();
  std::shared_ptr<Window> findFocusedOrSingleWindow() const;
  void computeWindowPos();
  void maybeFetchExternalClipboard();

  static double getPlatformTimerValue();

#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
  bool onGamepadConnectionChange(EmscriptenGamepadEvent const *iEvent);
#endif

private:
  struct ClipboardStringCallback
  {
    emscripten_glfw_clipboard_string_fun fCallback{};
    void *fUserData{};
  };

private:
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  std::vector<std::shared_ptr<Window>> fWindows{};
#else
  std::shared_ptr<Window> fSingleWindow{};
#endif
  GLFWwindow *fCurrentWindowOpaquePtr{};
  std::shared_ptr<Window> fCurrentWindow{}; // window made current via glfwMakeContextCurrent
  std::shared_ptr<Monitor> fCurrentMonitor{new Monitor{}};
  GLFWwindow *fLastKnownFocusedWindow{};
  Config fConfig{};
  float fScale{1.0f};
  double fInitialTime{getPlatformTimerValue()};

  // clipboard
  std::optional<std::string> fInternalClipboardText{};
  std::vector<std::promise<ClipboardString>> fExternalClipboardTextPromises{};
  std::vector<ClipboardStringCallback> fExternalClipboardTextCallbacks{};
  double fExternalClipboardRequestTime{};

  std::optional<Window::FullscreenRequest> fFullscreenRequest{};
  std::optional<Window::PointerLockRequest> fPointerLockRequest{};
  std::optional<Window::PointerUnlockRequest> fPointerUnlockRequest{};

  GLFWmonitorfun fMonitorCallback{};
  GLFWjoystickfun fJoystickCallback{};

  EventListener<EmscriptenMouseEvent> fOnMouseMove{};
  EventListener<EmscriptenMouseEvent> fOnMouseButtonUp{};
  EventListener<EmscriptenFullscreenChangeEvent> fOnFullscreenChange{};
  EventListener<EmscriptenPointerlockChangeEvent> fOnPointerLockChange{};
  EventListener<void> fOnPointerLockError{};

  int fSuperPlusKeyTimeout{525}; // milliseconds

#ifndef EMSCRIPTEN_GLFW3_DISABLE_JOYSTICK
  int fPresentJoystickCount{};
  EventListener<EmscriptenGamepadEvent> fOnGamepadConnected{};
  EventListener<EmscriptenGamepadEvent> fOnGamepadDisconnected{};
#endif
};

}

#endif //EMSCRIPTEN_GLFW_CONTEXT_H