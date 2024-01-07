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

#include <GLFW/glfw3.h>
#include <cstdio>
#include "emscripten/glfw3/Context.h"
#include <stdexcept>
#include <memory>
#include "emscripten/glfw3/ErrorHandler.h"
#include "emscripten/glfw3/Keyboard.h"

using namespace emscripten::glfw3;

//! not_implemented
[[noreturn]] static void not_implemented() { throw std::logic_error("not implemented"); }

//! logNotImplemented
static void logNotImplemented(char const *iFunction)
{
  ErrorHandler::instance().logWarning("%s is not implemented ont this platform\n", iFunction);
}

//! Unique context
static std::unique_ptr<Context> kContext{};

//! checkContextInitialized
static inline bool checkContextInitialized()
{
  if(!kContext)
  {
    ErrorHandler::instance().logError(GLFW_NOT_INITIALIZED, "GLFW has not been initialized");
    return false;
  }
  return true;
}

//! getContext
static inline Context *getContext() {
  if(!checkContextInitialized())
    return nullptr;
  else
    return kContext.get();
}

//! kLastRequestedGLFWWindow
static GLFWwindow* kLastRequestedGLFWWindow{};
//! kLastRequestedWindow
static std::shared_ptr<Window> kLastRequestedWindow{};

//! getWindow
static inline std::shared_ptr<Window> getWindow(GLFWwindow* iWindow)
{
  if(!checkContextInitialized())
  {
    return nullptr;
  }
  else
  {
    if(kLastRequestedGLFWWindow == iWindow && !kLastRequestedWindow->isDestroyed())
      return kLastRequestedWindow;

    auto window = kContext->getWindow(iWindow);
    if(window)
    {
      kLastRequestedGLFWWindow = iWindow;
      kLastRequestedWindow = window;
    }
    else
    {
      kLastRequestedGLFWWindow = nullptr;
      kLastRequestedWindow = nullptr;
    }

    return window;
  }
}

//! getMonitor
static inline std::shared_ptr<Monitor> getMonitor(GLFWmonitor* iMonitor) {
  if(!checkContextInitialized())
    return nullptr;
  else
    return kContext->getMonitor(iMonitor);
}

//! getJoystick
static inline Joystick *getJoystick(glfw_joystick_id_t id)
{
  if(!checkContextInitialized())
    return nullptr;
  else
    return Joystick::findJoystick(id);
}

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------
// glfwInit
//------------------------------------------------------------------------
GLFWAPI int glfwInit()
{
  printf("glfwInit()\n");
  if(kContext)
    return GLFW_TRUE;
  kContext = Context::init();
  return toGlfwBool((bool) kContext);
}

//------------------------------------------------------------------------
// glfwTerminate
//------------------------------------------------------------------------
GLFWAPI void glfwTerminate(void)
{
  kContext = nullptr;
  kLastRequestedGLFWWindow = nullptr;
  kLastRequestedWindow = nullptr;
  printf("glfwTerminate()\n");
}

//------------------------------------------------------------------------
// glfwGetVersion
//------------------------------------------------------------------------
GLFWAPI void glfwGetVersion(int* major, int* minor, int* rev)
{
  *major = GLFW_VERSION_MAJOR;
  *minor = GLFW_VERSION_MINOR;
  *rev = GLFW_VERSION_REVISION;
}

//------------------------------------------------------------------------
// glfwGetVersionString
//------------------------------------------------------------------------
GLFWAPI const char* glfwGetVersionString(void)
{
#define mkstr(s) #s
  constexpr char const *kVersionString = mkstr(GLFW_VERSION_MAJOR) "." mkstr(GLFW_VERSION_MINOR) "." mkstr(GLFW_VERSION_REVISION);
  return kVersionString;
}

//------------------------------------------------------------------------
// glfwSetErrorCallback
//------------------------------------------------------------------------
GLFWAPI GLFWerrorfun glfwSetErrorCallback(GLFWerrorfun callback)
{
  return ErrorHandler::instance().setErrorCallback(callback);
}

//------------------------------------------------------------------------
// glfwGetError
//------------------------------------------------------------------------
GLFWAPI int glfwGetError(const char** description)
{
  return ErrorHandler::instance().popError(description);
}

//------------------------------------------------------------------------
// glfwCreateWindow
//------------------------------------------------------------------------
GLFWAPI GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
  auto context = getContext();
  if(context)
    return context->createWindow(width, height, title, monitor, share);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwDestroyWindow
//------------------------------------------------------------------------
GLFWAPI void glfwDestroyWindow(GLFWwindow* window)
{
  if(kLastRequestedGLFWWindow == window)
  {
    kLastRequestedGLFWWindow = nullptr;
    kLastRequestedWindow = nullptr;
  }
  auto context = getContext();
  if(context)
    context->destroyWindow(window);
}


//------------------------------------------------------------------------
// glfwSetWindowUserPointer
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowUserPointer(GLFWwindow* window, void* pointer)
{
  auto w = getWindow(window);
  if(w)
    w->setUserPointer(pointer);
}

//------------------------------------------------------------------------
// glfwGetWindowUserPointer
//------------------------------------------------------------------------
GLFWAPI void* glfwGetWindowUserPointer(GLFWwindow* window)
{
  auto w = getWindow(window);
  if(w)
    return w->getUserPointer();
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwWindowShouldClose
//------------------------------------------------------------------------
GLFWAPI int glfwWindowShouldClose(GLFWwindow* window)
{
  auto w = getWindow(window);
  if(w)
    return w->getShouldClose();
  else
    return GLFW_TRUE;
}

//------------------------------------------------------------------------
// glfwSetWindowShouldClose
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowShouldClose(GLFWwindow* window, int value)
{
  auto w = getWindow(window);
  if(w)
    return w->setShouldClose(value);
}

//------------------------------------------------------------------------
// glfwMakeContextCurrent
//------------------------------------------------------------------------
GLFWAPI void glfwMakeContextCurrent(GLFWwindow* window)
{
  auto context = getContext();
  if(context)
    context->makeContextCurrent(window);
}

//------------------------------------------------------------------------
// glfwGetCurrentContext
//------------------------------------------------------------------------
GLFWAPI GLFWwindow* glfwGetCurrentContext(void)
{
  auto context = getContext();
  if(context)
    return context->getCurrentContext();
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwInitHint
//------------------------------------------------------------------------
GLFWAPI void glfwInitHint(int hint, int value) {  /** no init hint applies to this impl **/ }

//------------------------------------------------------------------------
// glfwDefaultWindowHints
//------------------------------------------------------------------------
GLFWAPI void glfwDefaultWindowHints(void)
{
  auto context = getContext();
  if(context)
    context->defaultWindowHints();
}

//------------------------------------------------------------------------
// glfwWindowHint
//------------------------------------------------------------------------
GLFWAPI void glfwWindowHint(int hint, int value)
{
  auto context = getContext();
  if(context)
    context->setWindowHint(hint, value);
}

//------------------------------------------------------------------------
// glfwWindowHintString
//------------------------------------------------------------------------
GLFWAPI void glfwWindowHintString(int hint, const char* value)
{
  auto context = getContext();
  if(context)
    context->setWindowHint(hint, value);
}

//------------------------------------------------------------------------
// glfwGetWindowContentScale
//------------------------------------------------------------------------
GLFWAPI void glfwGetWindowContentScale(GLFWwindow* window, float* xscale, float* yscale)
{
  auto w = getWindow(window);
  if(w)
    w->getContentScale(xscale, yscale);
}

//------------------------------------------------------------------------
// glfwSetWindowContentScaleCallback
//------------------------------------------------------------------------
GLFWAPI GLFWwindowcontentscalefun glfwSetWindowContentScaleCallback(GLFWwindow* window, GLFWwindowcontentscalefun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setContentScaleCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwSetWindowSize
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowSize(GLFWwindow* window, int width, int height)
{
  auto w = getWindow(window);
  if(w)
    w->setSize(width, height);
}

//------------------------------------------------------------------------
// glfwGetWindowSize
//------------------------------------------------------------------------
GLFWAPI void glfwGetWindowSize(GLFWwindow* window, int* width, int* height)
{
  auto w = getWindow(window);
  if(w)
    w->getSize(width, height);
}

//------------------------------------------------------------------------
// glfwGetFramebufferSize
//------------------------------------------------------------------------
GLFWAPI void glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height)
{
  auto w = getWindow(window);
  if(w)
    w->getFramebufferSize(width, height);
}

//------------------------------------------------------------------------
// glfwSetWindowSizeCallback
//------------------------------------------------------------------------
GLFWAPI GLFWwindowsizefun glfwSetWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setSizeCallback(callback);
  else
    return nullptr;

}

//------------------------------------------------------------------------
// glfwSetFramebufferSizeCallback
//------------------------------------------------------------------------
GLFWAPI GLFWframebuffersizefun glfwSetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setFramebufferSizeCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwGetCursorPos
//------------------------------------------------------------------------
GLFWAPI void glfwGetCursorPos(GLFWwindow* window, double* xpos, double* ypos)
{
  auto w = getWindow(window);
  if(w)
    w->getCursorPos(xpos, ypos);
}

//------------------------------------------------------------------------
// glfwSetCursorPosCallback
//------------------------------------------------------------------------
GLFWAPI GLFWcursorposfun glfwSetCursorPosCallback(GLFWwindow* window, GLFWcursorposfun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setCursorPosCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwGetMouseButton
//------------------------------------------------------------------------
GLFWAPI int glfwGetMouseButton(GLFWwindow* window, int button)
{
  auto w = getWindow(window);
  if(w)
    return w->getMouseButtonState(button);
  else
    return GLFW_RELEASE;
}

//------------------------------------------------------------------------
// glfwSetMouseButtonCallback
//------------------------------------------------------------------------
GLFWAPI GLFWmousebuttonfun glfwSetMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setMouseButtonCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwSetScrollCallback
//------------------------------------------------------------------------
GLFWAPI GLFWscrollfun glfwSetScrollCallback(GLFWwindow* window, GLFWscrollfun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setScrollCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwSetCursorEnterCallback
//------------------------------------------------------------------------
GLFWAPI GLFWcursorenterfun glfwSetCursorEnterCallback(GLFWwindow* window, GLFWcursorenterfun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setCursorEnterCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwCreateStandardCursor
//------------------------------------------------------------------------
GLFWAPI GLFWcursor* glfwCreateStandardCursor(int shape)
{
  auto context = getContext();
  if(context)
    return context->createStandardCursor(shape);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwSetCursor
//------------------------------------------------------------------------
GLFWAPI void glfwSetCursor(GLFWwindow* window, GLFWcursor* cursor)
{
  auto w = getWindow(window);
  if(w)
    w->setCursor(cursor);
}

//------------------------------------------------------------------------
// glfwSetKeyCallback
//------------------------------------------------------------------------
GLFWAPI GLFWkeyfun glfwSetKeyCallback(GLFWwindow* window, GLFWkeyfun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setKeyCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwSetCharCallback
//------------------------------------------------------------------------
GLFWAPI GLFWcharfun glfwSetCharCallback(GLFWwindow* window, GLFWcharfun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setCharCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwGetKeyScancode
//------------------------------------------------------------------------
GLFWAPI int glfwGetKeyScancode(int key)
{
  if(!checkContextInitialized())
    return -1;
  return Keyboard::getKeyScancode(key);
}

//------------------------------------------------------------------------
// glfwGetKeyName
//------------------------------------------------------------------------
GLFWAPI const char* glfwGetKeyName(int key, int scancode)
{
  if(!checkContextInitialized())
    return nullptr;
  return Keyboard::getKeyName(key, scancode);
}

//------------------------------------------------------------------------
// glfwGetKey
//------------------------------------------------------------------------
GLFWAPI int glfwGetKey(GLFWwindow* window, int key)
{
  auto w = getWindow(window);
  if(w)
    return w->getKeyState(key);
  else
    return GLFW_RELEASE;
}

//------------------------------------------------------------------------
// glfwSetWindowFocusCallback
//------------------------------------------------------------------------
GLFWAPI GLFWwindowfocusfun glfwSetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback)
{
  auto w = getWindow(window);
  if(w)
    return w->setFocusCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwFocusWindow
//------------------------------------------------------------------------
GLFWAPI void glfwFocusWindow(GLFWwindow* window)
{
  auto w = getWindow(window);
  if(w)
    w->focus();
}

//------------------------------------------------------------------------
// glfwGetInputMode
//------------------------------------------------------------------------
GLFWAPI int glfwGetInputMode(GLFWwindow* window, int mode)
{
  auto w = getWindow(window);
  if(w)
    return w->getInputMode(mode);
  else
    return GLFW_FALSE;
}

//------------------------------------------------------------------------
// glfwSetInputMode
//------------------------------------------------------------------------
GLFWAPI void glfwSetInputMode(GLFWwindow* window, int mode, int value)
{
  auto w = getWindow(window);
  if(w)
    w->setInputMode(mode, value);
}

//------------------------------------------------------------------------
// glfwGetWindowOpacity
//------------------------------------------------------------------------
GLFWAPI float glfwGetWindowOpacity(GLFWwindow* window)
{
  auto w = getWindow(window);
  if(w)
    return w->getOpacity();
  else
    return 1.0f;
}

//------------------------------------------------------------------------
// glfwGetWindowOpacity
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowOpacity(GLFWwindow* window, float opacity)
{
  auto w = getWindow(window);
  if(w)
    w->setOpacity(opacity);
}

//------------------------------------------------------------------------
// glfwShowWindow
//------------------------------------------------------------------------
GLFWAPI void glfwShowWindow(GLFWwindow* window)
{
  auto w = getWindow(window);
  if(w)
    w->setVisibility(true);
}

//------------------------------------------------------------------------
// glfwHideWindow
//------------------------------------------------------------------------
GLFWAPI void glfwHideWindow(GLFWwindow* window)
{
  auto w = getWindow(window);
  if(w)
    w->setVisibility(false);
}

//------------------------------------------------------------------------
// glfwGetWindowAttrib
//------------------------------------------------------------------------
GLFWAPI int glfwGetWindowAttrib(GLFWwindow* window, int attrib)
{
  auto w = getWindow(window);
  if(w)
    return w->getAttrib(attrib);
  else
    return 0;
}

//------------------------------------------------------------------------
// glfwSetWindowAttrib
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowAttrib(GLFWwindow* window, int attrib, int value)
{
  auto w = getWindow(window);
  if(w)
    w->setAttrib(attrib, value);
}

//------------------------------------------------------------------------
// glfwRawMouseMotionSupported
//------------------------------------------------------------------------
GLFWAPI int glfwRawMouseMotionSupported()
{
  return GLFW_FALSE;
}

//------------------------------------------------------------------------
// glfwGetMonitors
//------------------------------------------------------------------------
GLFWAPI GLFWmonitor** glfwGetMonitors(int* count)
{
  auto context = getContext();
  if(context)
    return context->getMonitors(count);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwGetPrimaryMonitor
//------------------------------------------------------------------------
GLFWAPI GLFWmonitor* glfwGetPrimaryMonitor(void)
{
  auto context = getContext();
  if(context)
    return context->getPrimaryMonitor();
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwGetMonitorPos
//------------------------------------------------------------------------
GLFWAPI void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos)
{
  auto context = getContext();
  if(context)
    context->getMonitorPos(monitor, xpos, ypos);
}

//------------------------------------------------------------------------
// glfwGetMonitorWorkarea
//------------------------------------------------------------------------
GLFWAPI void glfwGetMonitorWorkarea(GLFWmonitor* monitor, int* xpos, int* ypos, int* width, int* height)
{
  auto context = getContext();
  if(context)
    context->getMonitorWorkArea(monitor, xpos, ypos, width, height);
}

//------------------------------------------------------------------------
// glfwSetMonitorUserPointer
//------------------------------------------------------------------------
GLFWAPI void glfwSetMonitorUserPointer(GLFWmonitor* monitor, void* pointer)
{
  auto m = getMonitor(monitor);
  if(m)
    m->setUserPointer(pointer);
}

//------------------------------------------------------------------------
// glfwGetMonitorUserPointer
//------------------------------------------------------------------------
GLFWAPI void* glfwGetMonitorUserPointer(GLFWmonitor* monitor)
{
  auto m = getMonitor(monitor);
  if(m)
    return m->getUserPointer();
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwGetMonitorUserPointer
//------------------------------------------------------------------------
GLFWAPI GLFWmonitorfun glfwSetMonitorCallback(GLFWmonitorfun callback)
{
  auto context = getContext();
  if(context)
    return context->setMonitorCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwGetTime
//------------------------------------------------------------------------
GLFWAPI double glfwGetTime(void)
{
  auto context = getContext();
  if(context)
    return context->getTimeInSeconds();
  else
    return 0;
}

//------------------------------------------------------------------------
// glfwPollEvents
//------------------------------------------------------------------------
GLFWAPI void glfwPollEvents()
{
  auto context = getContext();
  if(context)
    context->pollEvents();
}

//------------------------------------------------------------------------
// glfwSetJoystickUserPointer
//------------------------------------------------------------------------
GLFWAPI void glfwSetJoystickUserPointer(int jid, void* pointer)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    joystick->setUserPointer(pointer);
}

//------------------------------------------------------------------------
// glfwGetJoystickUserPointer
//------------------------------------------------------------------------
GLFWAPI void* glfwGetJoystickUserPointer(int jid)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    return joystick->getUserPointer();
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwJoystickPresent
//------------------------------------------------------------------------
GLFWAPI int glfwJoystickPresent(int jid)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    return joystick->isPresent();
  else
    return GLFW_FALSE;
}

//------------------------------------------------------------------------
// glfwGetJoystickName
//------------------------------------------------------------------------
GLFWAPI const char* glfwGetJoystickName(int jid)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    return joystick->getName();
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwGetJoystickGUID
//------------------------------------------------------------------------
GLFWAPI const char* glfwGetJoystickGUID(int jid)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    return joystick->getMapping();
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwSetJoystickCallback
//------------------------------------------------------------------------
GLFWAPI GLFWjoystickfun glfwSetJoystickCallback(GLFWjoystickfun callback)
{
  auto context = getContext();
  if(context)
    return context->setJoystickCallback(callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwGetJoystickAxes
//------------------------------------------------------------------------
GLFWAPI const float* glfwGetJoystickAxes(int jid, int* count)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    return joystick->getAxes(count);
  else
  {
    *count = 0;
    return nullptr;
  }
}

//------------------------------------------------------------------------
// glfwGetJoystickButtons
//------------------------------------------------------------------------
GLFWAPI const unsigned char* glfwGetJoystickButtons(int jid, int* count)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    return joystick->getDigitalButtons(count);
  else
  {
    *count = 0;
    return nullptr;
  }
}

//------------------------------------------------------------------------
// glfwGetJoystickHats
//------------------------------------------------------------------------
GLFWAPI const unsigned char* glfwGetJoystickHats(int jid, int* count)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    return joystick->getHats(count);
  else
  {
    *count = 0;
    return nullptr;
  }
}

//------------------------------------------------------------------------
// glfwJoystickIsGamepad
//------------------------------------------------------------------------
GLFWAPI int glfwJoystickIsGamepad(int jid)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    return joystick->isGamepad();
  else
    return GLFW_FALSE;
}

//------------------------------------------------------------------------
// glfwUpdateGamepadMappings
//------------------------------------------------------------------------
GLFWAPI int glfwUpdateGamepadMappings(const char* string)
{
  logNotImplemented("glfwUpdateGamepadMappings");
  return GLFW_FALSE;
}

//------------------------------------------------------------------------
// glfwGetGamepadName
//------------------------------------------------------------------------
GLFWAPI const char* glfwGetGamepadName(int jid)
{
  auto joystick = getJoystick(jid);
  if(joystick && joystick->isGamepad())
    return joystick->getName();
  return nullptr;
}

//------------------------------------------------------------------------
// glfwGetGamepadState
//------------------------------------------------------------------------
GLFWAPI int glfwGetGamepadState(int jid, GLFWgamepadstate* state)
{
  auto joystick = getJoystick(jid);
  if(joystick)
    return joystick->getGamepadState(state);
  else
    return GLFW_FALSE;
}

//------------------------------------------------------------------------
// TODO Implement
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// glfwGetWindowPos
//------------------------------------------------------------------------
GLFWAPI void glfwGetWindowPos(GLFWwindow* window, int* xpos, int* ypos)
{
  // TODO implement
  *xpos = 0;
  *ypos = 0;
}


//------------------------------------------------------------------------
// no implementation for the emscripten platform
//------------------------------------------------------------------------
GLFWAPI const GLFWvidmode* glfwGetVideoModes(GLFWmonitor* monitor, int* count)
{
  *count = 0;
  return nullptr;
}
GLFWAPI const GLFWvidmode* glfwGetVideoMode(GLFWmonitor* monitor) { return nullptr; }
GLFWAPI GLFWcharmodsfun glfwSetCharModsCallback(GLFWwindow* window, GLFWcharmodsfun callback)
{
  ErrorHandler::instance().logWarning("glfwSetCharModsCallback is deprecated.");
  return nullptr;
}
GLFWAPI GLFWcursor* glfwCreateCursor(const GLFWimage* image, int xhot, int yhot)
{
  logNotImplemented("glfwCreateCursor");
  return nullptr;
}
GLFWAPI void glfwDestroyCursor(GLFWcursor* cursor) { logNotImplemented("glfwDestroyCursor"); }

//------------------------------------------------------------------------
// not_implemented
//------------------------------------------------------------------------
GLFWAPI void glfwGetMonitorPhysicalSize(GLFWmonitor* monitor, int* widthMM, int* heightMM){ not_implemented(); }
GLFWAPI void glfwGetMonitorContentScale(GLFWmonitor* monitor, float* xscale, float* yscale){ not_implemented(); }
GLFWAPI const char* glfwGetMonitorName(GLFWmonitor* monitor){ not_implemented(); }
GLFWAPI void glfwSetGamma(GLFWmonitor* monitor, float gamma){ not_implemented(); }
GLFWAPI const GLFWgammaramp* glfwGetGammaRamp(GLFWmonitor* monitor){ not_implemented(); }
GLFWAPI void glfwSetGammaRamp(GLFWmonitor* monitor, const GLFWgammaramp* ramp){ not_implemented(); }
GLFWAPI void glfwSetWindowTitle(GLFWwindow* window, const char* title){ not_implemented(); }
GLFWAPI void glfwSetWindowIcon(GLFWwindow* window, int count, const GLFWimage* images){ not_implemented(); }
GLFWAPI void glfwSetWindowPos(GLFWwindow* window, int xpos, int ypos){ not_implemented(); }
GLFWAPI void glfwSetWindowSizeLimits(GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight){ not_implemented(); }
GLFWAPI void glfwSetWindowAspectRatio(GLFWwindow* window, int numer, int denom){ not_implemented(); }
GLFWAPI void glfwGetWindowFrameSize(GLFWwindow* window, int* left, int* top, int* right, int* bottom){ not_implemented(); }
GLFWAPI void glfwIconifyWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwRestoreWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwMaximizeWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwRequestWindowAttention(GLFWwindow* window){ not_implemented(); }
GLFWAPI GLFWmonitor* glfwGetWindowMonitor(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwSetWindowMonitor(GLFWwindow* window, GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate){ not_implemented(); }
GLFWAPI GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback){ not_implemented(); }
GLFWAPI GLFWwindowclosefun glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback){ not_implemented(); }
GLFWAPI GLFWwindowrefreshfun glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback){ not_implemented(); }
GLFWAPI GLFWwindowiconifyfun glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback){ not_implemented(); }
GLFWAPI GLFWwindowmaximizefun glfwSetWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback){ not_implemented(); }
GLFWAPI void glfwWaitEvents(void){ not_implemented(); }
GLFWAPI void glfwWaitEventsTimeout(double timeout){ not_implemented(); }
GLFWAPI void glfwPostEmptyEvent(void){ not_implemented(); }
GLFWAPI void glfwSetCursorPos(GLFWwindow* window, double xpos, double ypos){ not_implemented(); }
GLFWAPI GLFWdropfun glfwSetDropCallback(GLFWwindow* window, GLFWdropfun callback){ not_implemented(); }
GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char* string){ not_implemented(); }
GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwSetTime(double time){ not_implemented(); }
GLFWAPI uint64_t glfwGetTimerValue(void){ not_implemented(); }
GLFWAPI uint64_t glfwGetTimerFrequency(void){ not_implemented(); }
GLFWAPI void glfwSwapBuffers(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwSwapInterval(int interval){ not_implemented(); }
GLFWAPI int glfwExtensionSupported(const char* extension){ not_implemented(); }
// GLFWAPI GLFWglproc glfwGetProcAddress(const char* procname){ not_implemented(); } implemented by emscripten GL
GLFWAPI int glfwVulkanSupported(void){ not_implemented(); }
GLFWAPI const char** glfwGetRequiredInstanceExtensions(uint32_t* count){ not_implemented(); }



#ifdef __cplusplus
} // extern "C"
#endif
