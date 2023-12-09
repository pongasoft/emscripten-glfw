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
  kContext = std::move(emscripten::glfw3::Context::init());
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
// glfwWindowShouldClose
//------------------------------------------------------------------------
GLFWAPI int glfwWindowShouldClose(GLFWwindow* window)
{
  auto context = getContext();
  if(context)
    return context->windowShouldClose(window);
  else
    return GLFW_TRUE;
}

//------------------------------------------------------------------------
// glfwSetWindowShouldClose
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowShouldClose(GLFWwindow* window, int value)
{
  auto context = getContext();
  if(context)
    return context->setWindowShouldClose(window, value);
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
  auto context = getContext();
  if(context)
    context->getWindowContentScale(window, xscale, yscale);
}

//------------------------------------------------------------------------
// glfwSetWindowContentScaleCallback
//------------------------------------------------------------------------
GLFWAPI GLFWwindowcontentscalefun glfwSetWindowContentScaleCallback(GLFWwindow* window, GLFWwindowcontentscalefun callback)
{
  auto context = getContext();
  if(context)
    return context->setWindowContentScaleCallback(window, callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// glfwSetWindowSize
//------------------------------------------------------------------------
GLFWAPI void glfwSetWindowSize(GLFWwindow* window, int width, int height)
{
  auto context = getContext();
  if(context)
    context->setWindowSize(window, width, height);
}

//------------------------------------------------------------------------
// glfwGetWindowSize
//------------------------------------------------------------------------
GLFWAPI void glfwGetWindowSize(GLFWwindow* window, int* width, int* height)
{
  auto context = getContext();
  if(context)
    context->getWindowSize(window, width, height);
}

//------------------------------------------------------------------------
// glfwGetFramebufferSize
//------------------------------------------------------------------------
GLFWAPI void glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height)
{
  auto context = getContext();
  if(context)
    context->getFramebufferSize(window, width, height);

}

//------------------------------------------------------------------------
// glfwSetWindowSizeCallback
//------------------------------------------------------------------------
GLFWAPI GLFWwindowsizefun glfwSetWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback)
{
  auto context = getContext();
  if(context)
    return context->setWindowSizeCallback(window, callback);
  else
    return nullptr;

}

//------------------------------------------------------------------------
// glfwSetFramebufferSizeCallback
//------------------------------------------------------------------------
GLFWAPI GLFWframebuffersizefun glfwSetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback)
{
  auto context = getContext();
  if(context)
    return context->setFramebufferSizeCallback(window, callback);
  else
    return nullptr;
}

//------------------------------------------------------------------------
// not_implemented
//------------------------------------------------------------------------
GLFWAPI GLFWmonitor** glfwGetMonitors(int* count){ not_implemented(); }
GLFWAPI GLFWmonitor* glfwGetPrimaryMonitor(void){ not_implemented(); }
GLFWAPI void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos){ not_implemented(); }
GLFWAPI void glfwGetMonitorWorkarea(GLFWmonitor* monitor, int* xpos, int* ypos, int* width, int* height){ not_implemented(); }
GLFWAPI void glfwGetMonitorPhysicalSize(GLFWmonitor* monitor, int* widthMM, int* heightMM){ not_implemented(); }
GLFWAPI void glfwGetMonitorContentScale(GLFWmonitor* monitor, float* xscale, float* yscale){ not_implemented(); }
GLFWAPI const char* glfwGetMonitorName(GLFWmonitor* monitor){ not_implemented(); }
GLFWAPI void glfwSetMonitorUserPointer(GLFWmonitor* monitor, void* pointer){ not_implemented(); }
GLFWAPI void* glfwGetMonitorUserPointer(GLFWmonitor* monitor){ not_implemented(); }
GLFWAPI GLFWmonitorfun glfwSetMonitorCallback(GLFWmonitorfun callback){ not_implemented(); }
GLFWAPI const GLFWvidmode* glfwGetVideoModes(GLFWmonitor* monitor, int* count){ not_implemented(); }
GLFWAPI const GLFWvidmode* glfwGetVideoMode(GLFWmonitor* monitor){ not_implemented(); }
GLFWAPI void glfwSetGamma(GLFWmonitor* monitor, float gamma){ not_implemented(); }
GLFWAPI const GLFWgammaramp* glfwGetGammaRamp(GLFWmonitor* monitor){ not_implemented(); }
GLFWAPI void glfwSetGammaRamp(GLFWmonitor* monitor, const GLFWgammaramp* ramp){ not_implemented(); }
GLFWAPI void glfwSetWindowTitle(GLFWwindow* window, const char* title){ not_implemented(); }
GLFWAPI void glfwSetWindowIcon(GLFWwindow* window, int count, const GLFWimage* images){ not_implemented(); }
GLFWAPI void glfwGetWindowPos(GLFWwindow* window, int* xpos, int* ypos){ not_implemented(); }
GLFWAPI void glfwSetWindowPos(GLFWwindow* window, int xpos, int ypos){ not_implemented(); }
GLFWAPI void glfwSetWindowSizeLimits(GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight){ not_implemented(); }
GLFWAPI void glfwSetWindowAspectRatio(GLFWwindow* window, int numer, int denom){ not_implemented(); }
GLFWAPI void glfwGetWindowFrameSize(GLFWwindow* window, int* left, int* top, int* right, int* bottom){ not_implemented(); }
GLFWAPI float glfwGetWindowOpacity(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwSetWindowOpacity(GLFWwindow* window, float opacity){ not_implemented(); }
GLFWAPI void glfwIconifyWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwRestoreWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwMaximizeWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwShowWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwHideWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwFocusWindow(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwRequestWindowAttention(GLFWwindow* window){ not_implemented(); }
GLFWAPI GLFWmonitor* glfwGetWindowMonitor(GLFWwindow* window){ not_implemented(); }
GLFWAPI void glfwSetWindowMonitor(GLFWwindow* window, GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate){ not_implemented(); }
GLFWAPI int glfwGetWindowAttrib(GLFWwindow* window, int attrib){ not_implemented(); }
GLFWAPI void glfwSetWindowAttrib(GLFWwindow* window, int attrib, int value){ not_implemented(); }
GLFWAPI void glfwSetWindowUserPointer(GLFWwindow* window, void* pointer){ not_implemented(); }
GLFWAPI void* glfwGetWindowUserPointer(GLFWwindow* window){ not_implemented(); }
GLFWAPI GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback){ not_implemented(); }
GLFWAPI GLFWwindowclosefun glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback){ not_implemented(); }
GLFWAPI GLFWwindowrefreshfun glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback){ not_implemented(); }
GLFWAPI GLFWwindowfocusfun glfwSetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback){ not_implemented(); }
GLFWAPI GLFWwindowiconifyfun glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback){ not_implemented(); }
GLFWAPI GLFWwindowmaximizefun glfwSetWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback){ not_implemented(); }
GLFWAPI void glfwPollEvents(void){ not_implemented(); }
GLFWAPI void glfwWaitEvents(void){ not_implemented(); }
GLFWAPI void glfwWaitEventsTimeout(double timeout){ not_implemented(); }
GLFWAPI void glfwPostEmptyEvent(void){ not_implemented(); }
GLFWAPI int glfwGetInputMode(GLFWwindow* window, int mode){ not_implemented(); }
GLFWAPI void glfwSetInputMode(GLFWwindow* window, int mode, int value){ not_implemented(); }
GLFWAPI int glfwRawMouseMotionSupported(void){ not_implemented(); }
GLFWAPI const char* glfwGetKeyName(int key, int scancode){ not_implemented(); }
GLFWAPI int glfwGetKeyScancode(int key){ not_implemented(); }
GLFWAPI int glfwGetKey(GLFWwindow* window, int key){ not_implemented(); }
GLFWAPI int glfwGetMouseButton(GLFWwindow* window, int button){ not_implemented(); }
GLFWAPI void glfwGetCursorPos(GLFWwindow* window, double* xpos, double* ypos){ not_implemented(); }
GLFWAPI void glfwSetCursorPos(GLFWwindow* window, double xpos, double ypos){ not_implemented(); }
GLFWAPI GLFWcursor* glfwCreateCursor(const GLFWimage* image, int xhot, int yhot){ not_implemented(); }
GLFWAPI GLFWcursor* glfwCreateStandardCursor(int shape){ not_implemented(); }
GLFWAPI void glfwDestroyCursor(GLFWcursor* cursor){ not_implemented(); }
GLFWAPI void glfwSetCursor(GLFWwindow* window, GLFWcursor* cursor){ not_implemented(); }
GLFWAPI GLFWkeyfun glfwSetKeyCallback(GLFWwindow* window, GLFWkeyfun callback){ not_implemented(); }
GLFWAPI GLFWcharfun glfwSetCharCallback(GLFWwindow* window, GLFWcharfun callback){ not_implemented(); }
GLFWAPI GLFWcharmodsfun glfwSetCharModsCallback(GLFWwindow* window, GLFWcharmodsfun callback){ not_implemented(); }
GLFWAPI GLFWmousebuttonfun glfwSetMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun callback){ not_implemented(); }
GLFWAPI GLFWcursorposfun glfwSetCursorPosCallback(GLFWwindow* window, GLFWcursorposfun callback){ not_implemented(); }
GLFWAPI GLFWcursorenterfun glfwSetCursorEnterCallback(GLFWwindow* window, GLFWcursorenterfun callback){ not_implemented(); }
GLFWAPI GLFWscrollfun glfwSetScrollCallback(GLFWwindow* window, GLFWscrollfun callback){ not_implemented(); }
GLFWAPI GLFWdropfun glfwSetDropCallback(GLFWwindow* window, GLFWdropfun callback){ not_implemented(); }
GLFWAPI int glfwJoystickPresent(int jid){ not_implemented(); }
GLFWAPI const float* glfwGetJoystickAxes(int jid, int* count){ not_implemented(); }
GLFWAPI const unsigned char* glfwGetJoystickButtons(int jid, int* count){ not_implemented(); }
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
GLFWAPI double glfwGetTime(void){ not_implemented(); }
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
