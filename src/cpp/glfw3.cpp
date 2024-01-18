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

#include <GLFW/emscripten_glfw3.h>
#include <cstdio>
#include "emscripten/glfw3/Context.h"
#include <memory>
#include "emscripten/glfw3/ErrorHandler.h"
#include "emscripten/glfw3/Keyboard.h"

using namespace emscripten::glfw3;

//! logNotImplemented
inline static void logNotImplemented(char const *iFunction)
{
#ifndef EMSCRIPTEN_GLFW3_DISABLE_WARNING
  ErrorHandler::instance().logWarning("%s is not implemented ont this platform\n", iFunction);
#endif
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
  constexpr char const *kVersionString =
    "Emscripten/WebAssembly GLFW " mkstr(GLFW_VERSION_MAJOR) "." mkstr(GLFW_VERSION_MINOR) "." mkstr(GLFW_VERSION_REVISION);
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
    w->setSize({width, height});
}

//------------------------------------------------------------------------
// glfwSetWindowSizeLimits
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowSizeLimits(GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight)
{
  auto w = getWindow(window);
  if(w)
    w->setSizeLimits(minwidth, minheight, maxwidth, maxheight);
}

//------------------------------------------------------------------------
// glfwSetWindowAspectRatio
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowAspectRatio(GLFWwindow* window, int numer, int denom)
{
  auto w = getWindow(window);
  if(w)
    w->setAspectRatio(numer, denom);
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

//! glfwSetCursorPos can't move the mouse pointer in javascript
GLFWAPI void glfwSetCursorPos(GLFWwindow* window, double xpos, double ypos) { logNotImplemented("glfwSetCursorPos"); }

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
// glfwGetWindowPos
//------------------------------------------------------------------------
GLFWAPI void glfwGetWindowPos(GLFWwindow* window, int* xpos, int* ypos)
{
  auto w = getWindow(window);
  if(w)
    w->getPosition(xpos, ypos);
}

//! glfwSetWindowPos (there is no generic way to set a canvas position)
GLFWAPI void glfwSetWindowPos(GLFWwindow* window, int xpos, int ypos) { logNotImplemented("glfwSetWindowPos"); }

//------------------------------------------------------------------------
// glfwSetWindowTitle
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowTitle(GLFWwindow* window, const char* title)
{
  auto context = getContext();
  if(context)
    context->setWindowTitle(window, title);
}

//------------------------------------------------------------------------
// glfwSetWindowPosCallback
//------------------------------------------------------------------------
GLFWAPI GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback)
{
  // there is no direct way in javascript to get notified => this would require polling so not implementing
  // at this stage
  logNotImplemented("glfwSetWindowPosCallback");
  return callback;
}

//------------------------------------------------------------------------
// glfwGetWindowFrameSize
//------------------------------------------------------------------------
GLFWAPI void glfwGetWindowFrameSize(GLFWwindow* window, int* left, int* top, int* right, int* bottom)
{
  // because a window is a canvas in this implementation, there is no edge
  getWindow(window);
  if(left) *left = 0;
  if(top) *top = 0;
  if(right) *right = 0;
  if(bottom) *bottom = 0;
}

//------------------------------------------------------------------------
// emscripten_glfw_set_next_window_canvas_selector
//------------------------------------------------------------------------
void emscripten_glfw_set_next_window_canvas_selector(char const *canvasSelector)
{
  auto context = getContext();
  if(context)
    context->setNextWindowCanvasSelector(canvasSelector);
}

//------------------------------------------------------------------------
// emscripten_glfw_is_window_fullscreen
//------------------------------------------------------------------------
int emscripten_glfw_is_window_fullscreen(GLFWwindow* window)
{
  auto w = getWindow(window);
  if(w)
    return toGlfwBool(w->isFullscreen());
  else
    return GLFW_FALSE;
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
// glfwGetMonitorPhysicalSize
//------------------------------------------------------------------------
GLFWAPI void glfwGetMonitorPhysicalSize(GLFWmonitor* monitor, int* widthMM, int* heightMM)
{
  logNotImplemented("glfwGetMonitorPhysicalSize");
}

//------------------------------------------------------------------------
// glfwGetMonitorContentScale
//------------------------------------------------------------------------
GLFWAPI void glfwGetMonitorContentScale(GLFWmonitor* monitor, float* xscale, float* yscale)
{
  auto context = getContext();
  if(context)
    context->getMonitorContentScale(monitor, xscale, yscale);
}

//------------------------------------------------------------------------
// glfwGetMonitorName
//------------------------------------------------------------------------
GLFWAPI const char* glfwGetMonitorName(GLFWmonitor* monitor)
{
  auto m = getMonitor(monitor);
  if(m)
    return m->getName();
  else
    return nullptr;
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
// glfwGetWindowMonitor
//------------------------------------------------------------------------
GLFWAPI GLFWmonitor* glfwGetWindowMonitor(GLFWwindow* window)
{
  auto context = getContext();
  if(context)
    return context->getMonitor(window);
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
// glfwSetTime
//------------------------------------------------------------------------
GLFWAPI void glfwSetTime(double time)
{
  auto context = getContext();
  if(context)
    context->setTimeInSeconds(time);
}

//------------------------------------------------------------------------
// glfwGetTimerValue
//------------------------------------------------------------------------
GLFWAPI uint64_t glfwGetTimerValue()
{
  return Context::getTimerValue();
}

//------------------------------------------------------------------------
// glfwGetTimerFrequency
//------------------------------------------------------------------------
GLFWAPI uint64_t glfwGetTimerFrequency()
{
  return kTimerFrequency;
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
// glfwSwapInterval
//------------------------------------------------------------------------
GLFWAPI void glfwSwapInterval(int interval)
{
  auto context = getContext();
  if(context)
    context->swapInterval(interval);
}

//! glfwWaitEvents no access to even loop in emscripten/javascript
GLFWAPI void glfwWaitEvents(void) { logNotImplemented("glfwWaitEvents"); }

//! glfwWaitEventsTimeout no access to even loop in emscripten/javascript
GLFWAPI void glfwWaitEventsTimeout(double timeout) { logNotImplemented("glfwWaitEventsTimeout"); }

//! glfwPostEmptyEvent no access to even loop in emscripten/javascript
GLFWAPI void glfwPostEmptyEvent(void) { logNotImplemented("glfwPostEmptyEvent"); }


//------------------------------------------------------------------------
// glfwExtensionSupported
//------------------------------------------------------------------------
GLFWAPI int glfwExtensionSupported(const char* extension)
{
  auto context = getContext();
  if(context)
    return context->isExtensionSupported(extension);
  return GLFW_FALSE;
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
// no implementation for the emscripten platform
//------------------------------------------------------------------------
GLFWAPI const GLFWvidmode* glfwGetVideoModes(GLFWmonitor* monitor, int* count)
{
  logNotImplemented("glfwGetVideoModes");
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
GLFWAPI void glfwSetGamma(GLFWmonitor* monitor, float gamma) { logNotImplemented("glfwSetGamma"); }
GLFWAPI const GLFWgammaramp* glfwGetGammaRamp(GLFWmonitor* monitor) { logNotImplemented("glfwGetGammaRamp"); return nullptr; }
GLFWAPI void glfwSetGammaRamp(GLFWmonitor* monitor, const GLFWgammaramp* ramp) { logNotImplemented("glfwSetGammaRamp"); }
GLFWAPI void glfwSetWindowIcon(GLFWwindow* window, int count, const GLFWimage* images) { logNotImplemented("glfwSetWindowIcon"); }
GLFWAPI void glfwIconifyWindow(GLFWwindow* window) { logNotImplemented("glfwIconifyWindow"); }
GLFWAPI void glfwRestoreWindow(GLFWwindow* window) { logNotImplemented("glfwRestoreWindow"); }
GLFWAPI void glfwMaximizeWindow(GLFWwindow* window) { logNotImplemented("glfwMaximizeWindow"); }
GLFWAPI void glfwRequestWindowAttention(GLFWwindow* window) { logNotImplemented("glfwRequestWindowAttention"); }
GLFWAPI void glfwSetWindowMonitor(GLFWwindow* window, GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate) { logNotImplemented("glfwSetWindowMonitor"); }
// per documentation: The close callback is not triggered by glfwDestroyWindow so this callback can never be called on this platform since there is no "user attempts to close the window" action
GLFWAPI GLFWwindowclosefun glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback) { logNotImplemented("glfwSetWindowCloseCallback"); return callback; }
GLFWAPI GLFWwindowrefreshfun glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback) { logNotImplemented("GLFWwindowrefreshfun"); return callback; }
GLFWAPI GLFWwindowiconifyfun glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback) { logNotImplemented("glfwSetWindowIconifyCallback"); return callback; }
GLFWAPI GLFWwindowmaximizefun glfwSetWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback) { logNotImplemented("glfwSetWindowMaximizeCallback"); return callback; }
GLFWAPI GLFWdropfun glfwSetDropCallback(GLFWwindow* window, GLFWdropfun callback) { logNotImplemented("glfwSetDropCallback"); return callback; }
GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char* string) { logNotImplemented("glfwSetClipboardString"); }
GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window) { logNotImplemented("glfwGetClipboardString"); return nullptr; }
GLFWAPI void glfwSwapBuffers(GLFWwindow* window) { logNotImplemented("glfwSwapBuffers"); }
// GLFWAPI GLFWglproc glfwGetProcAddress(const char* procname) { logNotImplemented("glfwGetProcAddress"); } implemented by emscripten GL
GLFWAPI int glfwVulkanSupported(void) { return GLFW_FALSE; }
GLFWAPI const char** glfwGetRequiredInstanceExtensions(uint32_t* count) { logNotImplemented("glfwGetRequiredInstanceExtensions"); *count = 0; return nullptr; }


#ifdef __cplusplus
} // extern "C"
#endif
