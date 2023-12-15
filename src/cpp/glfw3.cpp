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

[[noreturn]] static void not_implemented() { throw std::logic_error("not implemented"); }

static std::unique_ptr<emscripten::glfw3::Context> kContext{};
static inline emscripten::glfw3::Context *getContext() {
  if(!kContext)
    emscripten::glfw3::ErrorHandler::instance().logError(GLFW_NOT_INITIALIZED, "GLFW has not been initialized");
  return kContext.get();
}
static inline std::shared_ptr<emscripten::glfw3::Window> getWindow(GLFWwindow* iWindow) {
  if(!kContext)
  {
    emscripten::glfw3::ErrorHandler::instance().logError(GLFW_NOT_INITIALIZED, "GLFW has not been initialized");
    return nullptr;
  }
  else
  {
    return kContext->getWindow(iWindow);
  }
}
static inline std::shared_ptr<emscripten::glfw3::Monitor> getMonitor(GLFWmonitor* iMonitor) {
  if(!kContext)
  {
    emscripten::glfw3::ErrorHandler::instance().logError(GLFW_NOT_INITIALIZED, "GLFW has not been initialized");
    return nullptr;
  }
  else
  {
    return kContext->getMonitor(iMonitor);
  }
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
  kContext = emscripten::glfw3::Context::init();
  return GLFW_TRUE;
}

//------------------------------------------------------------------------
// glfwTerminate
//------------------------------------------------------------------------
GLFWAPI void glfwTerminate(void)
{
  kContext = nullptr;
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
  return emscripten::glfw3::ErrorHandler::instance().setErrorCallback(callback);
}

//------------------------------------------------------------------------
// glfwGetError
//------------------------------------------------------------------------
GLFWAPI int glfwGetError(const char** description)
{
  return emscripten::glfw3::ErrorHandler::instance().popError(description);
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
    context->windowHint(hint, value);
}

//------------------------------------------------------------------------
// glfwWindowHintString
//------------------------------------------------------------------------
GLFWAPI void glfwWindowHintString(int hint, const char* value)
{
  auto context = getContext();
  if(context)
    context->windowHint(hint, value);
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
    return w->getMouseButton(button);
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
    return context->getMonitorPos(monitor, xpos, ypos);
}

//------------------------------------------------------------------------
// glfwGetMonitorWorkarea
//------------------------------------------------------------------------
GLFWAPI void glfwGetMonitorWorkarea(GLFWmonitor* monitor, int* xpos, int* ypos, int* width, int* height)
{
  auto context = getContext();
  if(context)
    return context->getMonitorWorkArea(monitor, xpos, ypos, width, height);
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
// TODO Implement
//------------------------------------------------------------------------

GLFWAPI GLFWwindowfocusfun glfwSetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback)
{
  // TODO implement
  return nullptr;
}

GLFWAPI GLFWcursorenterfun glfwSetCursorEnterCallback(GLFWwindow* window, GLFWcursorenterfun callback)
{
  // TODO implement
  return nullptr;
}


GLFWAPI GLFWscrollfun glfwSetScrollCallback(GLFWwindow* window, GLFWscrollfun callback)
{
  // TODO implement
  return nullptr;
}

GLFWAPI GLFWkeyfun glfwSetKeyCallback(GLFWwindow* window, GLFWkeyfun callback)
{
  // TODO implement
  return nullptr;
}

GLFWAPI GLFWcharfun glfwSetCharCallback(GLFWwindow* window, GLFWcharfun callback)
{
  // TODO implement
  return nullptr;
}

GLFWAPI GLFWmonitorfun glfwSetMonitorCallback(GLFWmonitorfun callback)
{
  // TODO implement
  return nullptr;
}

GLFWAPI int glfwGetKey(GLFWwindow* window, int key)
{
  // TODO implement
  return GLFW_RELEASE;
}


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
// glfwGetInputMode
//------------------------------------------------------------------------
GLFWAPI int glfwGetInputMode(GLFWwindow* window, int mode)
{
  // TODO fix mock implementation!
  if(mode == GLFW_CURSOR)
    return GLFW_CURSOR_NORMAL;
  else
    return GLFW_FALSE;
}

//------------------------------------------------------------------------
// glfwSetInputMode
//------------------------------------------------------------------------
GLFWAPI void glfwSetInputMode(GLFWwindow* window, int mode, int value)
{
  // TODO implement

}

//------------------------------------------------------------------------
// glfwSetCursor
//------------------------------------------------------------------------
GLFWAPI void glfwSetCursor(GLFWwindow* window, GLFWcursor* cursor)
{
  // TODO: use canvas.style.cursor = value ???
}

//------------------------------------------------------------------------
// glfwGetJoystickAxes
//------------------------------------------------------------------------
GLFWAPI const float* glfwGetJoystickAxes(int jid, int* count)
{
  *count = 0;
  return nullptr;
}

//------------------------------------------------------------------------
// glfwGetJoystickButtons
//------------------------------------------------------------------------
GLFWAPI const unsigned char* glfwGetJoystickButtons(int jid, int* count)
{
  *count = 0;
  return nullptr;
}

//------------------------------------------------------------------------
// no implementation for the emscripten platform
//------------------------------------------------------------------------
GLFWAPI void glfwShowWindow(GLFWwindow* window) { }
GLFWAPI void glfwHideWindow(GLFWwindow* window) { }
GLFWAPI GLFWcursor* glfwCreateStandardCursor(int shape) { return nullptr; }
GLFWAPI const GLFWvidmode* glfwGetVideoModes(GLFWmonitor* monitor, int* count)
{
  *count = 0;
  return nullptr;
}
GLFWAPI const GLFWvidmode* glfwGetVideoMode(GLFWmonitor* monitor) { return nullptr; }
GLFWAPI void glfwPollEvents(void){ }

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
GLFWAPI float glfwGetWindowOpacity(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwSetWindowOpacity(GLFWwindow* window, float opacity){ not_implemented(); }
GLFWAPI void glfwIconifyWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwRestoreWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwMaximizeWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwFocusWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwRequestWindowAttention(GLFWwindow* window){ not_implemented(); }
GLFWAPI GLFWmonitor* glfwGetWindowMonitor(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwSetWindowMonitor(GLFWwindow* window, GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate){ not_implemented(); }
GLFWAPI int glfwGetWindowAttrib(GLFWwindow* window, int attrib){ not_implemented(); }
GLFWAPI void glfwSetWindowAttrib(GLFWwindow* window, int attrib, int value){ not_implemented(); }
GLFWAPI GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback){ not_implemented(); }
GLFWAPI GLFWwindowclosefun glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback){ not_implemented(); }
GLFWAPI GLFWwindowrefreshfun glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback){ not_implemented(); }
GLFWAPI GLFWwindowiconifyfun glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback){ not_implemented(); }
GLFWAPI GLFWwindowmaximizefun glfwSetWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback){ not_implemented(); }
GLFWAPI void glfwWaitEvents(void){ not_implemented(); }
GLFWAPI void glfwWaitEventsTimeout(double timeout){ not_implemented(); }
GLFWAPI void glfwPostEmptyEvent(void){ not_implemented(); }
GLFWAPI int glfwRawMouseMotionSupported(void){ not_implemented(); }
GLFWAPI const char* glfwGetKeyName(int key, int scancode){ not_implemented(); }
GLFWAPI int glfwGetKeyScancode(int key){ not_implemented(); }
GLFWAPI void glfwSetCursorPos(GLFWwindow* window, double xpos, double ypos){ not_implemented(); }
GLFWAPI GLFWcursor* glfwCreateCursor(const GLFWimage* image, int xhot, int yhot){ not_implemented(); }
GLFWAPI void glfwDestroyCursor(GLFWcursor* cursor){ not_implemented(); }
GLFWAPI GLFWcharmodsfun glfwSetCharModsCallback(GLFWwindow* window, GLFWcharmodsfun callback){ not_implemented(); }
GLFWAPI GLFWdropfun glfwSetDropCallback(GLFWwindow* window, GLFWdropfun callback){ not_implemented(); }
GLFWAPI int glfwJoystickPresent(int jid){ not_implemented(); }
GLFWAPI const unsigned char* glfwGetJoystickHats(int jid, int* count){ not_implemented(); }
GLFWAPI const char* glfwGetJoystickName(int jid){ not_implemented(); }
GLFWAPI const char* glfwGetJoystickGUID(int jid){ not_implemented(); }
GLFWAPI void glfwSetJoystickUserPointer(int jid, void* pointer){ not_implemented(); }
GLFWAPI void* glfwGetJoystickUserPointer(int jid){ not_implemented(); }
GLFWAPI int glfwJoystickIsGamepad(int jid){ not_implemented(); }
GLFWAPI GLFWjoystickfun glfwSetJoystickCallback(GLFWjoystickfun callback){ not_implemented(); }
GLFWAPI int glfwUpdateGamepadMappings(const char* string){ not_implemented(); }
GLFWAPI const char* glfwGetGamepadName(int jid){ not_implemented(); }
GLFWAPI int glfwGetGamepadState(int jid, GLFWgamepadstate* state){ not_implemented(); }
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
