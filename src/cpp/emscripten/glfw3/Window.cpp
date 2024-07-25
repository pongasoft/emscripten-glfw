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
#include "../../../../include/GLFW/emscripten_glfw3.h"
#include <utility>
#include "ErrorHandler.h"
#include "Cursor.h"
#include <algorithm>

extern "C" {
void emscripten_glfw3_context_set_title(char const *iTitle);
void emscripten_glfw3_window_destroy(GLFWwindow *iWindow);
void emscripten_glfw3_window_set_size(GLFWwindow *iWindow, int iWidth, int iHeight, int iFramebufferWidth, int iFramebufferHeight);
void emscripten_glfw3_window_get_position(GLFWwindow *iWindow, int *oX, int *oY);
void emscripten_glfw3_window_focus(GLFWwindow *iWindow);
void emscripten_glfw3_window_set_cursor(GLFWwindow *iWindow, char const *iCursor);
float emscripten_glfw3_window_get_computed_opacity(GLFWwindow *iWindow);
void emscripten_glfw3_window_set_opacity(GLFWwindow *iWindow, float iOpacity);
bool emscripten_glfw3_window_get_computed_visibility(GLFWwindow *iWindow);
void emscripten_glfw3_window_set_visibility(GLFWwindow *iWindow, bool iVisible);
void emscripten_glfw3_context_gl_init(GLFWwindow *iWindow);
void emscripten_glfw3_context_gl_bool_attribute(GLFWwindow *iWindow, char const *iAttributeName, bool iAttributeValue);
int emscripten_glfw3_context_gl_create_context(GLFWwindow *iWindow);
int emscripten_glfw3_context_gl_make_context_current(GLFWwindow *iWindow);
int emscripten_glfw3_context_make_canvas_resizable(GLFWwindow *window, char const *canvasResizeSelector, char const *handleSelector);
int emscripten_glfw3_context_unmake_canvas_resizable(GLFWwindow *window);
}

namespace emscripten::glfw3 {

static ErrorHandler &kErrorHandler = ErrorHandler::instance();

//------------------------------------------------------------------------
// Cursor::kCursors
// The "default" css cursor is set to `nullptr` on purpose so that the
// cursor property is removed, thus reverting to the "default" which can
// be overriden in CSS
//------------------------------------------------------------------------
const std::array<Cursor, 10> Cursor::kCursors = {
  Cursor{GLFW_ARROW_CURSOR, nullptr}, // default
  Cursor{GLFW_IBEAM_CURSOR, "text"},
  Cursor{GLFW_CROSSHAIR_CURSOR, "crosshair"},
  Cursor{GLFW_POINTING_HAND_CURSOR, "pointer"},
  Cursor{GLFW_RESIZE_EW_CURSOR, "ew-resize"},
  Cursor{GLFW_RESIZE_NS_CURSOR, "ns-resize"},
  Cursor{GLFW_RESIZE_NWSE_CURSOR, "nwse-resize"},
  Cursor{GLFW_RESIZE_NESW_CURSOR, "nesw-resize"},
  Cursor{GLFW_RESIZE_ALL_CURSOR, "all-scroll"},
  Cursor{GLFW_NOT_ALLOWED_CURSOR, "not-allowed"},
};

const Cursor Cursor::kCursorHidden{0, "none"};

//------------------------------------------------------------------------
// Window::Window
//------------------------------------------------------------------------
Window::Window(Context *iContext, Config iConfig, float iMonitorScale, char const *iTitle) :
  fContext{iContext},
  fConfig{std::move(iConfig)},
  fMonitorScale{iMonitorScale},
  fTitle{iTitle ? std::optional<std::string>{iTitle} : std::nullopt}
{
}

//------------------------------------------------------------------------
// Window::init
//------------------------------------------------------------------------
void Window::init(int iWidth, int iHeight)
{
  fOpacity = emscripten_glfw3_window_get_computed_opacity(asOpaquePtr());
  fVisible = emscripten_glfw3_window_get_computed_visibility(asOpaquePtr());

  if(fConfig.fVisible == GLFW_FALSE && fVisible)
    setVisibility(false);

  if(fConfig.fFocused)
  {
    focus();
    emscripten_glfw3_context_set_title(getTitle());
  }

  setCanvasSize({iWidth, iHeight});
}

//------------------------------------------------------------------------
// Window::~Window
//------------------------------------------------------------------------
Window::~Window()
{
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
    emscripten_glfw3_window_destroy(asOpaquePtr());
    fDestroyed = true;
  }
}

//------------------------------------------------------------------------
// Window::focus
//------------------------------------------------------------------------
void Window::focus()
{
  fFocused = true;
  emscripten_glfw3_window_focus(asOpaquePtr());
}

//------------------------------------------------------------------------
// Window::maybeRescale
//------------------------------------------------------------------------
bool Window::maybeRescale(std::function<void()> const &iAction)
{
  auto oldScale = getScale();
  iAction();
  auto newScale = getScale();
  auto scaleChanged = oldScale != newScale;
  if(scaleChanged)
  {
    setCanvasSize(fSize);
    if(fContentScaleCallback)
      fContentScaleCallback(asOpaquePtr(), newScale, newScale);
  }
  return scaleChanged;
}

//------------------------------------------------------------------------
// Window::setMonitorScale
//------------------------------------------------------------------------
void Window::setMonitorScale(float iScale)
{
  if(fMonitorScale != iScale)
  {
    maybeRescale([this, scale=iScale]() { fMonitorScale = scale; });
  }
}

//------------------------------------------------------------------------
// Window::maybeApplySizeConstraints
//------------------------------------------------------------------------
Vec2<int> Window::maybeApplySizeConstraints(Vec2<int> const &iSize) const
{
  if(isFullscreen() || !isResizable())
    return iSize;

  Vec2<int> size = iSize;

  if(fMinSize.width != GLFW_DONT_CARE && size.width < fMinSize.width)
    size.width = fMinSize.width;
  if(fMinSize.height != GLFW_DONT_CARE && size.height < fMinSize.height)
    size.height = fMinSize.height;

  if(fMaxSize.width != GLFW_DONT_CARE && size.width > fMaxSize.width)
    size.width = fMaxSize.width;
  if(fMaxSize.height != GLFW_DONT_CARE && size.height > fMaxSize.height)
    size.height = fMaxSize.height;

  if(fAspectRatioNumerator != GLFW_DONT_CARE && fAspectRatioDenominator != GLFW_DONT_CARE)
  {
    auto aspectRatio = static_cast<float>(fAspectRatioNumerator) / static_cast<float>(fAspectRatioDenominator);
    auto imageAspectRatio = static_cast<float>(size.width) / static_cast<float>(size.height);
    if(imageAspectRatio < aspectRatio)
      size.width = static_cast<int>(std::round(static_cast<float>(size.height) * aspectRatio));
    else
      size.height = static_cast<int>(std::round(static_cast<float>(size.width) / aspectRatio));
  }

  return size;
}

//------------------------------------------------------------------------
// Window::setSizeLimits
//------------------------------------------------------------------------
void Window::setSizeLimits(int iMinWidth, int iMinHeight, int iMaxWidth, int iMaxHeight)
{
  if((iMinWidth == GLFW_DONT_CARE || iMinWidth >= 0) &&
     (iMinHeight == GLFW_DONT_CARE || iMinHeight >= 0) &&
     (iMaxWidth == GLFW_DONT_CARE || (iMaxWidth >= 0 && iMaxWidth > iMinWidth)) &&
     iMaxHeight == GLFW_DONT_CARE || (iMaxHeight >= 0 && iMaxHeight > iMinHeight))
  {
    fMinSize = {iMinWidth, iMinHeight};
    fMaxSize = {iMaxWidth, iMaxHeight};

    auto newSize = maybeApplySizeConstraints(fSize);
    if(newSize != fSize)
      setSize(newSize);
  }
  else
  {
    kErrorHandler.logError(GLFW_INVALID_VALUE, "glfwSetWindowSizeLimits: Invalid parameter(s) (%d,%d,%d,%d)",
                           iMinWidth, iMinHeight, iMaxWidth, iMaxHeight);
  }
}

//------------------------------------------------------------------------
// Window::setAspectRatio
//------------------------------------------------------------------------
void Window::setAspectRatio(int iNumerator, int iDenominator)
{
  if(iNumerator == GLFW_DONT_CARE || iDenominator == GLFW_DONT_CARE)
  {
    fAspectRatioNumerator = GLFW_DONT_CARE;
    fAspectRatioDenominator = GLFW_DONT_CARE;
    return;
  }

  if(iNumerator <= 0 || iDenominator <= 0)
  {
    kErrorHandler.logError(GLFW_INVALID_VALUE, "glfwSetWindowAspectRatio: Invalid parameter(s) (%d,%d)",
                           iNumerator, iDenominator);
    return;
  }

  fAspectRatioNumerator = iNumerator;
  fAspectRatioDenominator = iDenominator;

  auto newSize = maybeApplySizeConstraints(fSize);
  if(newSize != fSize)
    setSize(newSize);
}

//------------------------------------------------------------------------
// Window::getPosition
//------------------------------------------------------------------------
void Window::getPosition(int *oX, int *oY)
{
  int x,y;
  emscripten_glfw3_window_get_position(asOpaquePtr(), &x, &y);
  if(oX)
    *oX = x;
  if(oY)
    *oY = y;
}

//------------------------------------------------------------------------
// Window::setCanvasSize
//------------------------------------------------------------------------
void Window::setCanvasSize(Vec2<int> const &iSize)
{
  // ignoring
  if(iSize.width <= 0 || iSize.height <= 0)
    return;

  auto sizeChanged = fSize != iSize;
  fSize = iSize;

  auto fbSize = iSize;

  if(isHiDPIAware())
  {
    fbSize.width = static_cast<int>(std::floor(static_cast<float>(iSize.width) * fMonitorScale));
    fbSize.height = static_cast<int>(std::floor(static_cast<float>(iSize.height) * fMonitorScale));
  }

  auto framebufferSizeChanged = fFramebufferSize != fbSize;

  fFramebufferSize = fbSize;

  emscripten_glfw3_window_set_size(asOpaquePtr(), fSize.width, fSize.height, fFramebufferSize.width, fFramebufferSize.height);

  if(sizeChanged)
  {
    // make sure that the cursor stays in the window
    setCursorPos({fMouse.fCursorPos.x, fMouse.fCursorPos.y});

    if(fSizeCallback)
      fSizeCallback(asOpaquePtr(), fSize.width, fSize.height);
  }

  if(framebufferSizeChanged && fFramebufferSizeCallback)
    fFramebufferSizeCallback(asOpaquePtr(), fFramebufferSize.width, fFramebufferSize.height);
}

//------------------------------------------------------------------------
// Window::resize
//------------------------------------------------------------------------
void Window::resize(Vec2<int> const &iSize)
{
  if(!isFullscreen() && isResizable())
    setSize(maybeApplySizeConstraints(iSize));
}

//------------------------------------------------------------------------
// Window::setCursorPos
//------------------------------------------------------------------------
void Window::setCursorPos(Vec2<double> const &iPos)
{
  // clamp to window when not in pointer lock mode
  auto pos = isPointerLock() ? iPos : Vec2<double>{std::clamp(iPos.x, 0.0, static_cast<double>(fSize.width)),
                                                   std::clamp(iPos.y, 0.0, static_cast<double>(fSize.height))};

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
  emscripten_glfw3_window_set_opacity(asOpaquePtr(), fOpacity);
}


//------------------------------------------------------------------------
// Window::setVisibility
//------------------------------------------------------------------------
void Window::setVisibility(bool iVisible)
{
  fVisible = iVisible;
  emscripten_glfw3_window_set_visibility(asOpaquePtr(), fVisible);
  if(fVisible && fConfig.fFocusOnShow)
    focus();
}

//------------------------------------------------------------------------
// Window::getAttrib
//------------------------------------------------------------------------
int Window::getAttrib(int iAttrib)
{
  switch(iAttrib)
  {
    case GLFW_VISIBLE:
      return toGlfwBool(fVisible);

    case GLFW_FOCUSED:
      return toGlfwBool(fFocused);

    case GLFW_FOCUS_ON_SHOW:
      return fConfig.fFocusOnShow;

    case GLFW_SCALE_TO_MONITOR:
      return fConfig.fScaleToMonitor;

    case GLFW_SCALE_FRAMEBUFFER:
      return fConfig.fScaleFramebuffer;

    case GLFW_RESIZABLE:
      return fConfig.fResizable;

    default:
      kErrorHandler.logWarning("glfwGetWindowAttrib: attrib [%d] not supported", iAttrib);
      return 0;
  }
}

//------------------------------------------------------------------------
// Window::setAttrib
//------------------------------------------------------------------------
void Window::setAttrib(int iAttrib, int iValue)
{
  switch(iAttrib)
  {
    case GLFW_VISIBLE:
      setVisibility(toCBool(iValue));
      break;

    case GLFW_FOCUSED:
      if(toCBool(iValue))
        focus();
      break;

    case GLFW_FOCUS_ON_SHOW:
      fConfig.fFocusOnShow = toGlfwBool(iValue);
      break;

    case GLFW_SCALE_TO_MONITOR:
      kErrorHandler.logWarning("GLFW_SCALE_TO_MONITOR is deprecated for this platform. Use GLFW_SCALE_FRAMEBUFFER instead.");
      maybeRescale([this, iValue]() { fConfig.fScaleToMonitor = toGlfwBool(iValue); });
      break;

    case GLFW_SCALE_FRAMEBUFFER:
      maybeRescale([this, iValue]() { fConfig.fScaleFramebuffer = toGlfwBool(iValue); });
      break;

    case GLFW_RESIZABLE:
      setResizable(toCBool(iValue));
      break;

    default:
      kErrorHandler.logWarning("glfwSetWindowAttrib: attrib [%d] not supported", iAttrib);
      break;
  }
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
  if(iXScale)
    *iXScale = scale;
  if(iYScale)
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

  if(fMouse.fButtonStates[iButton] == Mouse::kStickyPress)
  {
    fMouse.fButtonStates[iButton] = GLFW_RELEASE;
    return GLFW_PRESS;
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
    if(isPointerLock() || fMouse.isCursorHidden())
      fMouse.fVisibleCursor = cursor;
    else
    {
      fMouse.fCursor = cursor;
      emscripten_glfw3_window_set_cursor(asOpaquePtr(), cursor->fCSSValue);
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
      return toGlfwBool(fKeyboard.getStickyKeys());

    case GLFW_STICKY_MOUSE_BUTTONS:
      return fMouse.fStickyMouseButtons;

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

    case GLFW_STICKY_MOUSE_BUTTONS:
      fMouse.fStickyMouseButtons = toGlfwBool(iValue);
      if(fMouse.fStickyMouseButtons == GLFW_FALSE)
      {
        for(auto &state: fMouse.fButtonStates)
        {
          if(state == Mouse::kStickyPress)
            state = GLFW_RELEASE;
        }
      }
      break;

    case GLFW_STICKY_KEYS:
      fKeyboard.setStickyKeys(toCBool(iValue));
      break;

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

  if(iCursorMode == GLFW_CURSOR_CAPTURED)
  {
    kErrorHandler.logError(GLFW_FEATURE_UNAVAILABLE, "glfwSetInputMode: GLFW_CURSOR/GLFW_CURSOR_CAPTURED [%d] not supported", iCursorMode);
    return;
  }

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
      emscripten_glfw3_window_set_cursor(asOpaquePtr(), cursor->fCSSValue);
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
    emscripten_glfw3_window_set_cursor(asOpaquePtr(), cursor->fCSSValue);
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
// Window::onFocusChange
//------------------------------------------------------------------------
bool Window::onFocusChange(bool iFocus)
{
  fFocused = iFocus;
  if(!isFocused())
    fKeyboard.resetAllKeys(asOpaquePtr());
  else
  {
    fContext->onFocus(asOpaquePtr());
    emscripten_glfw3_context_set_title(getTitle());
  }
  if(fFocusCallback)
    fFocusCallback(asOpaquePtr(), toGlfwBool(isFocused()));
  return true;
}

//------------------------------------------------------------------------
// Window::onMouseButtonUp
//------------------------------------------------------------------------
bool Window::onMouseButtonUp(EmscriptenMouseEvent const *iMouseEvent)
{
  auto lastButton = emscriptenToGLFWButton(iMouseEvent->button);
  if(lastButton >= 0)
  {
    // up can happen even if mouse is outside the window
    if(fMouse.fButtonStates[lastButton] == GLFW_PRESS)
    {
      fMouse.fLastButton = lastButton;
      fMouse.fLastButtonState = GLFW_RELEASE;
      fMouse.fButtonStates[lastButton] = fMouse.fStickyMouseButtons ? Mouse::kStickyPress : GLFW_RELEASE;

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

  if(iAdd)
  {
    // fOnMouseMove
    fOnMouseMove
      .target(selector)
      .listener([this](int iEventType, const EmscriptenMouseEvent *iEvent) {
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
      })
      .add(emscripten_set_mousemove_callback_on_thread);

    // fOnMouseButtonDown
    fOnMouseButtonDown
      .target(selector)
      .listener([this](int iEventType, const EmscriptenMouseEvent *iEvent) {
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
      })
      .add(emscripten_set_mousedown_callback_on_thread);

    // fOnMouseEnter
    fOnMouseEnter
      .target(selector)
      .listener([this](int iEventType, const EmscriptenMouseEvent *iEvent) {
        if(fMouse.fCursorEnterCallback)
        {
          fMouse.fCursorEnterCallback(asOpaquePtr(), GLFW_TRUE);
          return true;
        }
        return false;
      })
      .add(emscripten_set_mouseenter_callback_on_thread);

    // fOnMouseLeave
    fOnMouseLeave
      .target(selector)
      .listener([this](int iEventType, const EmscriptenMouseEvent *iEvent) {
        if(fMouse.fCursorEnterCallback)
        {
          fMouse.fCursorEnterCallback(asOpaquePtr(), GLFW_FALSE);
          return true;
        }
        return false;
      })
      .add(emscripten_set_mouseleave_callback_on_thread);

    // fOnMouseWheel
    fOnMouseWheel
      .target(selector)
      .listener([this](int iEventType, const EmscriptenWheelEvent *iEvent) {
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
      })
      .add(emscripten_set_wheel_callback_on_thread);

    // keyboard
    // note: keyboard events are handled in context because target is "window"

    // fOnFocusChange
    fOnFocusChange
      .target(selector)
      .listener([this](int eventType, const EmscriptenFocusEvent *iEvent) { return onFocusChange(true); })
      .add(emscripten_set_focus_callback_on_thread);

    // fOnBlurChange
    fOnBlurChange
      .target(selector)
      .listener([this](int eventType, const EmscriptenFocusEvent *iEvent) { return onFocusChange(false); })
      .add(emscripten_set_blur_callback_on_thread);
  }
  else
  {
    fOnMouseMove.remove();
    fOnMouseButtonDown.remove();
    fOnMouseEnter.remove();
    fOnMouseLeave.remove();
    fOnMouseWheel.remove();
    fOnFocusChange.remove();
    fOnBlurChange.remove();
  }
}

//------------------------------------------------------------------------
// Window::onEnterFullscreen
//------------------------------------------------------------------------
void Window::onEnterFullscreen(std::optional<Vec2<int>> const &iScreenSize)
{
  fFullscreen = true;
  if(iScreenSize)
  {
    fSizeBeforeFullscreen = fSize;
    setSize(*iScreenSize);
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

  if((fSizeBeforeFullscreen && *fSizeBeforeFullscreen != fSize))
    setSize(*fSizeBeforeFullscreen);

  fSizeBeforeFullscreen = std::nullopt;

  return true;
}

//------------------------------------------------------------------------
// Window::makeCanvasResizable
//------------------------------------------------------------------------
int Window::makeCanvasResizable(std::string_view canvasResizeSelector, std::optional<std::string_view> handleSelector)
{
  return emscripten_glfw3_context_make_canvas_resizable(asOpaquePtr(),
                                                        canvasResizeSelector.data(),
                                                        handleSelector ? handleSelector->data() : nullptr);
}

//------------------------------------------------------------------------
// Window::unmakeCanvasResizable
//------------------------------------------------------------------------
int Window::unmakeCanvasResizable()
{
  return emscripten_glfw3_context_unmake_canvas_resizable(asOpaquePtr());
}


}