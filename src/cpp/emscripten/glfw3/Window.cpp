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

#include "Window.h"
#include "Context.h"
#include <emscripten/em_types.h>
#include <utility>
#include "ErrorHandler.h"
#include "Cursor.h"
#include <algorithm>

extern "C" {
void emscripten_glfw3_context_window_destroy(GLFWwindow *iWindow);
void emscripten_glfw3_context_window_set_size(GLFWwindow *iWindow, int iWidth, int iHeight, int iFramebufferWidth, int iFramebufferHeight);
void emscripten_glfw3_context_window_focus(GLFWwindow *iWindow);
void emscripten_glfw3_context_window_set_cursor(GLFWwindow *iWindow, char const *iCursor);
void emscripten_glfw3_context_window_set_opacity(GLFWwindow *iWindow, float iOpacity);
void emscripten_glfw3_context_gl_init(GLFWwindow *iWindow);
void emscripten_glfw3_context_gl_bool_attribute(GLFWwindow *iWindow, char const *iAttributeName, bool iAttributeValue);
int emscripten_glfw3_context_gl_create_context(GLFWwindow *iWindow);
int emscripten_glfw3_context_gl_make_context_current(GLFWwindow *iWindow);
}

namespace emscripten::glfw3 {

static ErrorHandler &kErrorHandler = ErrorHandler::instance();

//------------------------------------------------------------------------
// Cursor::kCursors
// The "default" css cursor is set to `nullptr` on purpose so that the
// cursor property is removed, thus reverting to the "default" which can
// be overriden in CSS
//------------------------------------------------------------------------
const std::array<Cursor, 6> Cursor::kCursors = {
  Cursor{GLFW_ARROW_CURSOR, nullptr}, // default
  Cursor{GLFW_IBEAM_CURSOR, "text"},
  Cursor{GLFW_CROSSHAIR_CURSOR, "crosshair"},
  Cursor{GLFW_HAND_CURSOR, "pointer"},
  Cursor{GLFW_HRESIZE_CURSOR, "ns-resize"},
  Cursor{GLFW_VRESIZE_CURSOR, "ew-resize"},
};

const Cursor Cursor::kCursorHidden{0, "none"};

//------------------------------------------------------------------------
// Window::Window
//------------------------------------------------------------------------
Window::Window(Context *iContext, Config iConfig, float iMonitorScale) :
  fContext{iContext},
  fConfig{std::move(iConfig)},
  fMonitorScale{iMonitorScale}
{
  printf("Window(%p)\n", asOpaquePtr());
  createEventListeners();
}

//------------------------------------------------------------------------
// Window::~Window
//------------------------------------------------------------------------
Window::~Window()
{
  printf("~Window(%p)\n", asOpaquePtr());
  destroy();
}

//------------------------------------------------------------------------
// Window::destroy
//------------------------------------------------------------------------
void Window::destroy()
{
  if(!isDestroyed())
  {
    addOrRemoveEventListeners(false);
    emscripten_glfw3_context_window_destroy(asOpaquePtr());
    fDestroyed = true;
  }
}

//------------------------------------------------------------------------
// Window::focus
//------------------------------------------------------------------------
void Window::focus()
{
  emscripten_glfw3_context_window_focus(asOpaquePtr());
}

//------------------------------------------------------------------------
// Window::setMonitorScale
//------------------------------------------------------------------------
void Window::setMonitorScale(float iScale)
{
  if(fMonitorScale != iScale)
  {
    auto oldScale = getScale();
    fMonitorScale = iScale;
    auto newScale = getScale();
    if(oldScale != newScale && fContentScaleCallback)
      fContentScaleCallback(asOpaquePtr(), fMonitorScale, fMonitorScale);
  }
}

//------------------------------------------------------------------------
// Window::setSize
//------------------------------------------------------------------------
void Window::setSize(int iWidth, int iHeight)
{
  auto sizeChanged = fWidth != iWidth || fHeight != iHeight;
  fWidth = iWidth;
  fHeight = iHeight;

  int fbWidth = iWidth;
  int fbHeight = iHeight;
  
  if(isHiDPIAware())
  {
    fbWidth = static_cast<int>(std::floor(static_cast<float>(iWidth) * fMonitorScale));
    fbHeight = static_cast<int>(std::floor(static_cast<float>(iHeight) * fMonitorScale));
  }

  auto framebufferSizeChanged = fFramebufferWidth != fbWidth || fFramebufferHeight != fbHeight;

  fFramebufferWidth = fbWidth;
  fFramebufferHeight = fbHeight;

  emscripten_glfw3_context_window_set_size(asOpaquePtr(), fWidth, fHeight, fFramebufferWidth, fFramebufferHeight);

  if(sizeChanged)
  {
    // make sure that the cursor stays in the window
    setCursorPos({fMouse.fCursorPos.x, fMouse.fCursorPos.y});

    if(fSizeCallback)
      fSizeCallback(asOpaquePtr(), fWidth, fHeight);
  }

  if(framebufferSizeChanged && fFramebufferSizeCallback)
    fFramebufferSizeCallback(asOpaquePtr(), fFramebufferWidth, fFramebufferHeight);
}

//------------------------------------------------------------------------
// Window::setCursorPos
//------------------------------------------------------------------------
void Window::setCursorPos(Vec2<double> const &iPos)
{
  // clamp to window when not in pointer lock mode
  auto pos = isPointerLock() ? iPos : Vec2<double>{std::clamp(iPos.x, 0.0, static_cast<double>(fWidth)),
                                                   std::clamp(iPos.y, 0.0, static_cast<double>(fHeight))};

  if(fMouse.fCursorPos != pos)
  {
    fMouse.fCursorPos = pos;
    if(fMouse.fCursorPosCallback)
      fMouse.fCursorPosCallback(asOpaquePtr(), fMouse.fCursorPos.x, fMouse.fCursorPos.y);
  }
}

//------------------------------------------------------------------------
// Window::setOpacity
//------------------------------------------------------------------------
void Window::setOpacity(float iOpacity)
{
  fOpacity = std::clamp(iOpacity, 0.0f, 1.0f);
  emscripten_glfw3_context_window_set_opacity(asOpaquePtr(), iOpacity);
}

//------------------------------------------------------------------------
// Window::createGLContext
//------------------------------------------------------------------------
bool Window::createGLContext()
{
  if(fConfig.fClientAPI != GLFW_NO_API)
  {
    auto id = asOpaquePtr();
    emscripten_glfw3_context_gl_init(id);
    emscripten_glfw3_context_gl_bool_attribute(id, "antialias", fConfig.fSamples > 0);
    emscripten_glfw3_context_gl_bool_attribute(id, "depth", fConfig.fDepthBits > 0);
    emscripten_glfw3_context_gl_bool_attribute(id, "stencil", fConfig.fStencilBits > 0);
    emscripten_glfw3_context_gl_bool_attribute(id, "alpha", fConfig.fAlphaBits > 0);

    if(emscripten_glfw3_context_gl_create_context(id) != EMSCRIPTEN_RESULT_SUCCESS)
    {
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "Cannot create GL context for [%s]", getCanvasSelector());
      return false;
    }

    fHasGLContext = true;
  }

  return true;
}

//------------------------------------------------------------------------
// Window::makeGLContextCurrent
//------------------------------------------------------------------------
void Window::makeGLContextCurrent()
{
  if(fHasGLContext)
    emscripten_glfw3_context_gl_make_context_current(asOpaquePtr());
}

//------------------------------------------------------------------------
// Window::getContentScale
//------------------------------------------------------------------------
void Window::getContentScale(float *iXScale, float *iYScale) const
{
  auto scale = getScale();
  *iXScale = scale;
  *iYScale = scale;
}

//------------------------------------------------------------------------
// Window::getMouseButtonState
//------------------------------------------------------------------------
glfw_mouse_button_state_t Window::getMouseButtonState(glfw_mouse_button_t iButton)
{
  if(iButton < 0 || iButton > GLFW_MOUSE_BUTTON_LAST)
  {
    kErrorHandler.logError(GLFW_INVALID_VALUE, "Invalid button [%d]", iButton);
    return GLFW_RELEASE;
  }
  return fMouse.fButtonStates[iButton];
}

//------------------------------------------------------------------------
// Window::setCursor
//------------------------------------------------------------------------
void Window::setCursor(GLFWcursor *iCursor)
{
  auto cursor = Cursor::findCursor(iCursor);
  if(cursor)
  {
    if(isPointerLock())
      fMouse.fVisibleCursor = cursor;
    else
    {
      fMouse.fCursor = cursor;
      emscripten_glfw3_context_window_set_cursor(asOpaquePtr(), cursor->fCSSValue);
    }
  }
  else
    kErrorHandler.logError(GLFW_INVALID_VALUE, "Invalid cursor");
}

//------------------------------------------------------------------------
// Window::getInputMode
//------------------------------------------------------------------------
int Window::getInputMode(int iMode) const
{
  switch(iMode)
  {
    case GLFW_CURSOR:
      return fMouse.fCursorMode;

    case GLFW_STICKY_KEYS:
    case GLFW_STICKY_MOUSE_BUTTONS:
    case GLFW_LOCK_KEY_MODS:
      kErrorHandler.logWarning("glfwGetInputMode: input mode [%d] not implemented yet", iMode);
      break;

    case GLFW_RAW_MOUSE_MOTION:
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "glfwGetInputMode: input mode [GLFW_RAW_MOUSE_MOTION] not supported");
      break;

    default:
      kErrorHandler.logError(GLFW_INVALID_ENUM, "glfwGetInputMode: Invalid mode [%d]", iMode);
      break;
  }
  return GLFW_FALSE;
}

//------------------------------------------------------------------------
// Window::setInputMode
//------------------------------------------------------------------------
void Window::setInputMode(int iMode, int iValue)
{
  switch(iMode)
  {
    case GLFW_CURSOR:
      setCursorMode(iValue);
      break;

    case GLFW_STICKY_KEYS:
    case GLFW_STICKY_MOUSE_BUTTONS:
    case GLFW_LOCK_KEY_MODS: // TODO: need to implement my own keyboard event to get this info
      kErrorHandler.logWarning("glfwSetInputMode: input mode [%d] not implemented yet", iMode);
      break;

    case GLFW_RAW_MOUSE_MOTION:
      // TODO: this requires a pending spec canvas.requestPointerLock({unadjustedMovement: true});
      //       Will implement when stable
      kErrorHandler.logError(GLFW_PLATFORM_ERROR, "glfwSetInputMode: input mode [GLFW_RAW_MOUSE_MOTION] not supported");
      break;

    default:
      kErrorHandler.logError(GLFW_INVALID_ENUM, "glfwSetInputMode: Invalid mode [%d]", iMode);
      break;
  }
}


//------------------------------------------------------------------------
// Window::setCursorMode
//------------------------------------------------------------------------
void Window::setCursorMode(glfw_cursor_mode_t iCursorMode)
{
  if(fMouse.fCursorMode == iCursorMode)
    return;

  if(iCursorMode == GLFW_CURSOR_DISABLED)
  {
    // note that this is asynchronous, so we do NOT update fMouse.fCursorMode here
    fContext->requestPointerLock(asOpaquePtr());
  }
  else
  {
    if(isPointerLock())
      // note that this is asynchronous, so we do NOT update fMouse.fCursorMode here
      fContext->requestPointerUnlock(asOpaquePtr(), iCursorMode);
    else
    {
      fMouse.fCursorMode = iCursorMode;
      auto const *cursor = iCursorMode == GLFW_CURSOR_HIDDEN ? fMouse.hideCursor() : fMouse.showCursor();
      emscripten_glfw3_context_window_set_cursor(asOpaquePtr(), cursor->fCSSValue);
    }
  }
}

//------------------------------------------------------------------------
// Window::onPointerLock
//------------------------------------------------------------------------
void Window::onPointerLock()
{
  if(isPointerLock())
    return;

  fMouse.hideCursor();

  fMouse.fCursorLockResidual= {};
  fMouse.fCursorPosBeforePointerLock = fMouse.fCursorPos;
  fMouse.fCursorMode = GLFW_CURSOR_DISABLED;
  setCursorPos({});
}

//------------------------------------------------------------------------
// Window::onPointerUnlock
//------------------------------------------------------------------------
bool Window::onPointerUnlock(std::optional<glfw_cursor_mode_t> iCursorMode)
{
  if(isPointerLock())
  {
    auto cursorMode = iCursorMode ? *iCursorMode : GLFW_CURSOR_NORMAL;
    fMouse.fCursorMode = cursorMode;
    auto const *cursor = iCursorMode == GLFW_CURSOR_HIDDEN ? fMouse.hideCursor() : fMouse.showCursor();
    emscripten_glfw3_context_window_set_cursor(asOpaquePtr(), cursor->fCSSValue);
    setCursorPos(fMouse.fCursorPosBeforePointerLock);
    return true;
  }
  return false;
}

//------------------------------------------------------------------------
// emscriptenToGLFWButton
//------------------------------------------------------------------------
inline glfw_mouse_button_t emscriptenToGLFWButton(unsigned short iEmscriptenButton)
{
  switch(iEmscriptenButton)
  {
    case 0:
      return GLFW_MOUSE_BUTTON_LEFT;
    case 1:
      return GLFW_MOUSE_BUTTON_MIDDLE;
    case 2:
      return GLFW_MOUSE_BUTTON_RIGHT;
    default:
      return -1;
  }
}

//------------------------------------------------------------------------
// Window::createEventListeners
//------------------------------------------------------------------------
void Window::createEventListeners()
{
  // fOnMouseMove
  fOnMouseMove = [this](int iEventType, const EmscriptenMouseEvent *iEvent) {
    Vec2<double> cursorPos{};
    if(isPointerLock())
    {
      fMouse.fCursorLockResidual.x += iEvent->movementX;
      fMouse.fCursorLockResidual.y += iEvent->movementY;
      cursorPos = fMouse.fCursorLockResidual;
      // following SDL implementation to not lose sub-pixel motion
      fMouse.fCursorLockResidual.x -= cursorPos.x;
      fMouse.fCursorLockResidual.y -= cursorPos.y;
    }
    else
    {
      cursorPos = {static_cast<double>(iEvent->targetX), static_cast<double>(iEvent->targetY)};
    }
    setCursorPos(cursorPos);
    return true;
  };

  // fOnMouseButtonDown
  fOnMouseButtonDown = [this](int iEventType, const EmscriptenMouseEvent *iEvent) {
    // TODO: implement GLFW_STICKY_MOUSE_BUTTONS
    auto lastButton = emscriptenToGLFWButton(iEvent->button);
    if(lastButton >= 0)
    {
      // down can only happen when inside the window
      fMouse.fLastButton = lastButton;
      fMouse.fLastButtonState = GLFW_PRESS;
      fMouse.fButtonStates[lastButton] = GLFW_PRESS;

      if(fFocusOnMouse && !isFocused())
        focus();

      if(fMouse.fButtonCallback)
        fMouse.fButtonCallback(asOpaquePtr(), fMouse.fLastButton, fMouse.fLastButtonState, fKeyboard.computeCallbackModifierBits());
    }
    return true;
  };

  // fOnMouseEnterOrLeave
  fOnMouseEnterOrLeave = [this](int iEventType, const EmscriptenMouseEvent *iEvent) {
    if(fMouse.fCursorEnterCallback)
    {
      fMouse.fCursorEnterCallback(asOpaquePtr(), toGlfwBool(iEventType == EMSCRIPTEN_EVENT_MOUSEENTER));
      return true;
    }
    return false;
  };

  // fOnMouseWheel
  fOnMouseWheel = [this](int iEventType, const EmscriptenWheelEvent *iEvent) {
    if(fMouse.fScrollCallback)
    {
      // Note: this code is copied/inspired by SDL implementation
      double multiplier;
      switch(iEvent->deltaMode)
      {
        case DOM_DELTA_PIXEL: multiplier = 1.0 / 100.0; break; // 100 pixels make up a step.
        case DOM_DELTA_LINE:  multiplier = 1.0 / 3.0;   break; // 3 lines make up a step.
        case DOM_DELTA_PAGE:  multiplier = 80.0;        break; // A page makes up 80 steps.
        default: return false; // should not happen
      }
      fMouse.fScrollCallback(asOpaquePtr(), iEvent->deltaX * -multiplier, iEvent->deltaY * -multiplier);
      return true;
    }
    return false;
  };

  // fOnFocusChange
  fOnFocusChange = [this](int eventType, const EmscriptenFocusEvent *iEvent) {
    fFocused = eventType == EMSCRIPTEN_EVENT_FOCUS;
    if(!isFocused())
      fKeyboard.resetAllKeys(asOpaquePtr());
    else
      fContext->onFocus(asOpaquePtr());
    if(fFocusCallback)
      fFocusCallback(asOpaquePtr(), toGlfwBool(isFocused()));
    return true;
  };
}

//------------------------------------------------------------------------
// Window::onMouseButtonUp
//------------------------------------------------------------------------
bool Window::onMouseButtonUp(EmscriptenMouseEvent const *iMouseEvent)
{
  // TODO: implement GLFW_STICKY_MOUSE_BUTTONS
  auto lastButton = emscriptenToGLFWButton(iMouseEvent->button);
  if(lastButton >= 0)
  {
    // up can happen even if mouse is outside the window
    if(fMouse.fButtonStates[lastButton] == GLFW_PRESS)
    {
      fMouse.fLastButton = lastButton;
      fMouse.fLastButtonState = GLFW_RELEASE;
      fMouse.fButtonStates[lastButton] = GLFW_RELEASE;

      if(fMouse.fButtonCallback)
        fMouse.fButtonCallback(asOpaquePtr(), fMouse.fLastButton, fMouse.fLastButtonState,fKeyboard.computeCallbackModifierBits());
    }
  }

  return true;
}

//------------------------------------------------------------------------
// Window::addOrRemoveEventListeners
//------------------------------------------------------------------------
void Window::addOrRemoveEventListeners(bool iAdd)
{
  auto selector = getCanvasSelector();
  printf("Window::addOrRemoveEventListeners(%s, %s)\n", selector, iAdd ? "true" : "false");

  // mouse
  addOrRemoveListener<EmscriptenMouseEvent>(emscripten_set_mousemove_callback_on_thread, iAdd, selector, &fOnMouseMove, false);
  addOrRemoveListener<EmscriptenMouseEvent>(emscripten_set_mousedown_callback_on_thread, iAdd, selector, &fOnMouseButtonDown, false);
  addOrRemoveListener<EmscriptenMouseEvent>(emscripten_set_mouseenter_callback_on_thread, iAdd, selector, &fOnMouseEnterOrLeave, false);
  addOrRemoveListener<EmscriptenMouseEvent>(emscripten_set_mouseleave_callback_on_thread, iAdd, selector, &fOnMouseEnterOrLeave, false);
  addOrRemoveListener<EmscriptenWheelEvent>(emscripten_set_wheel_callback_on_thread, iAdd, selector, &fOnMouseWheel, false);
  // note: mouseup_callback is registered with context because target is "document"

  // keyboard
  // note: keyboard events are handled in context because target is "window"

  // focus
  addOrRemoveListener<EmscriptenFocusEvent>(emscripten_set_focus_callback_on_thread, iAdd, selector, &fOnFocusChange, false);
  addOrRemoveListener<EmscriptenFocusEvent>(emscripten_set_blur_callback_on_thread, iAdd, selector, &fOnFocusChange, false);
}

//------------------------------------------------------------------------
// Window::onEnterFullscreen
//------------------------------------------------------------------------
void Window::onEnterFullscreen(std::optional<Vec2<int>> const &iScreenSize)
{
  fFullscreen = true;
  if(iScreenSize)
  {
    fWidthBeforeFullscreen = fWidth;
    fHeightBeforeFullscreen = fHeight;
    setSize(iScreenSize->width, iScreenSize->height);
  }
  focus();
}

//------------------------------------------------------------------------
// Window::onExitFullscreen
//------------------------------------------------------------------------
bool Window::onExitFullscreen()
{
  if(!fFullscreen)
    return false;

  fFullscreen = false;

  if((fWidthBeforeFullscreen && *fWidthBeforeFullscreen != fWidth) || (fHeightBeforeFullscreen && *fHeightBeforeFullscreen != fHeight))
    setSize(*fWidthBeforeFullscreen, *fHeightBeforeFullscreen);

  fWidthBeforeFullscreen = std::nullopt;
  fHeightBeforeFullscreen = std::nullopt;


  return true;
}


}