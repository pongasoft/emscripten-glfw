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

#ifndef EMSCRIPTEN_GLFW_MOUSE_H
#define EMSCRIPTEN_GLFW_MOUSE_H

#include <GLFW/glfw3.h>
#include <array>
#include <map>
#include "Types.h"
#include "Cursor.h"
#include "Config.h"

using glfw_mouse_button_state_t = int; // ex: GLFW_RELEASE
using glfw_mouse_button_t = int; // ex: GLFW_MOUSE_BUTTON_LEFT
using glfw_cursor_mode_t = int; // ex: GLFW_CURSOR_NORMAL

namespace emscripten::glfw3 {

class Window;

struct BitSet
{
  void set(unsigned short iBit) { fBits |= (1 << iBit); }
  void clear(unsigned short iBit) { fBits &= ~(1 << iBit); }
  void clear() { fBits = 0; }
  bool isSet(unsigned short iBit) const { return fBits & (1 << iBit); }

private:
  unsigned long fBits{};
};

class Mouse
{
public:

  constexpr bool isPointerLock() const { return fCursorMode == GLFW_CURSOR_DISABLED; }

  inline std::shared_ptr<Cursor> hideCursor() {
    fVisibleCursor = fCursor;
    fCursor = StandardCursor::getHiddenCursor();
    return fCursor;
  }

  inline std::shared_ptr<Cursor> showCursor() {
    fCursor = fVisibleCursor;
    return fCursor;
  }

  inline bool isCursorHidden() const { return fCursor == StandardCursor::getHiddenCursor(); }

  friend class Window;

public:
  struct Buttons
  {
    static_assert(sizeof(unsigned long) * 8 >= 32, "There should be enough buttons");

    void press(unsigned short iButton) { fButtons.set(iButton); }
    void release(unsigned short iButton, bool iIsSticky)
    {
      fButtons.clear(iButton);
      if(iIsSticky && iButton <= GLFW_MOUSE_BUTTON_LAST)
        fStickyButtons.set(iButton);
    }
    void releaseSticky(unsigned short iButton) { fStickyButtons.clear(iButton); }
    bool isPressed(unsigned short iButton) const { return fButtons.isSet(iButton); }
    bool isSticky(unsigned short iButton) const { return fStickyButtons.isSet(iButton); }
    void clearSticky() { fStickyButtons.clear(); }

  private:
    // at least 32 buttons which, while not being "unlimited", seems to be enough for the number of buttons for a mouse...
    BitSet fButtons;
    // only "known" GLFW buttons can be sticky (due to padding and alignment, using an unsigned long instead of
    // uint8_t is not wasting any space)
    BitSet fStickyButtons{};
  };

public:
  Buttons fButtons{};

  glfw_cursor_mode_t fCursorMode{GLFW_CURSOR_NORMAL};
  glfw_bool_t fStickyMouseButtons{GLFW_FALSE};
  glfw_bool_t fUnlimitedMouseButtons{GLFW_FALSE};

  Vec2<double> fCursorPos{};
  Vec2<double> fCursorPosBeforePointerLock{};
  Vec2<double> fCursorLockResidual{};

  GLFWmousebuttonfun fButtonCallback{};
  GLFWscrollfun fScrollCallback{};
  GLFWcursorenterfun fCursorEnterCallback{};
  GLFWcursorposfun fCursorPosCallback{};

private:
  std::shared_ptr<Cursor> fCursor{StandardCursor::getDefault()};
  std::shared_ptr<Cursor> fVisibleCursor{fCursor};
};

}

#endif //EMSCRIPTEN_GLFW_MOUSE_H
