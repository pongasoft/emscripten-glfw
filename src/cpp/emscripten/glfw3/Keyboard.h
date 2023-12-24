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

#ifndef EMSCRIPTEN_GLFW_KEYBOARD_H
#define EMSCRIPTEN_GLFW_KEYBOARD_H

#include <GLFW/glfw3.h>
#include <array>
#include <emscripten/html5.h>
#include "KeyboardMapping.h"

namespace emscripten::glfw3 {

using glfw_key_state_t = int; // ex: GLFW_RELEASE

class Keyboard
{
public:
  glfw_key_state_t getKeyState(glfw_key_t iKey) const;
//  bool isKeyPressed(glfw_key_t iKey) const { return getKeyState(iKey) != GLFW_RELEASE; }
  constexpr bool isShiftPressed() const { return isValidKeyPressed(GLFW_KEY_LEFT_SHIFT) || isValidKeyPressed(GLFW_KEY_RIGHT_SHIFT); }
  constexpr bool isControlPressed() const { return isValidKeyPressed(GLFW_KEY_LEFT_CONTROL) || isValidKeyPressed(GLFW_KEY_RIGHT_CONTROL); }
  constexpr bool isAltPressed() const { return isValidKeyPressed(GLFW_KEY_LEFT_ALT) || isValidKeyPressed(GLFW_KEY_RIGHT_ALT); }
  constexpr bool isSuperPressed() const { return isValidKeyPressed(GLFW_KEY_LEFT_SUPER) || isValidKeyPressed(GLFW_KEY_RIGHT_SUPER); }

  inline GLFWkeyfun setKeyCallback(GLFWkeyfun iCallback) { return std::exchange(fKeyCallback, iCallback); }
  inline GLFWcharfun setCharCallback(GLFWcharfun iCallback) { return std::exchange(fCharCallback, iCallback); }

  void setInputModeLockKeyMods(bool iValue) { fInputModeLockKeyMods = iValue; }
  int computeCallbackModifierBits(EmscriptenKeyboardEvent const *iKeyboardEvent = nullptr) const;

  bool onKeyDown(GLFWwindow *iWindow, const EmscriptenKeyboardEvent *iKeyboardEvent);
  bool onKeyUp(GLFWwindow *iWindow, const EmscriptenKeyboardEvent *iKeyboardEvent);
  void resetAllKeys(GLFWwindow *iWindow);

public:
  static constexpr glfw_scancode_t getKeyScancode(glfw_key_t iKey) { return keyboard::keyCodeToScancode(iKey); }
  static const char* getKeyName(glfw_key_t iKey, glfw_scancode_t iScancode);

private:
  static constexpr glfw_key_t getGLFWKey(glfw_scancode_t iScancode) { return keyboard::scancodeToKeyCode(iScancode); }
  static constexpr glfw_scancode_t getKeyScancode(char const *iKeyboardEventCode) { return keyboard::keyboardEventCodeToScancode(iKeyboardEventCode); }

  // constexpr when we know that the key is a valid key
  constexpr glfw_key_state_t getValidKeyState(glfw_key_t iKey) const { return fKeyStates[iKey]; }
  constexpr bool isValidKeyPressed(glfw_key_t iKey) const { return getValidKeyState(iKey) != GLFW_RELEASE; }

private:
  std::array<glfw_key_state_t, GLFW_KEY_LAST + 1> fKeyStates{GLFW_RELEASE};
  bool fInputModeLockKeyMods{};
  GLFWkeyfun fKeyCallback{};
  GLFWcharfun fCharCallback{};
};


}

#endif //EMSCRIPTEN_GLFW_KEYBOARD_H
