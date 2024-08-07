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
#include <map>
#include <emscripten/html5.h>
#include "KeyboardMapping.h"

namespace emscripten::glfw3 {

using glfw_key_state_t = int; // ex: GLFW_RELEASE

class Keyboard
{
public:
  struct Event
  {
    char const *code;
    char const *key;
    bool repeat;
    int codepoint;
    int modifierBits;

    constexpr bool isSuperPressed() const { return modifierBits & GLFW_MOD_SUPER; }
  };

public:
  glfw_key_state_t getKeyState(glfw_key_t iKey);
//  bool isKeyPressed(glfw_key_t iKey) const { return getKeyState(iKey) != GLFW_RELEASE; }
  constexpr bool isShiftPressed() const { return isValidKeyPressed(GLFW_KEY_LEFT_SHIFT) || isValidKeyPressed(GLFW_KEY_RIGHT_SHIFT); }
  constexpr bool isControlPressed() const { return isValidKeyPressed(GLFW_KEY_LEFT_CONTROL) || isValidKeyPressed(GLFW_KEY_RIGHT_CONTROL); }
  constexpr bool isAltPressed() const { return isValidKeyPressed(GLFW_KEY_LEFT_ALT) || isValidKeyPressed(GLFW_KEY_RIGHT_ALT); }
  constexpr bool isSuperPressed() const { return isValidKeyPressed(GLFW_KEY_LEFT_SUPER) || isValidKeyPressed(GLFW_KEY_RIGHT_SUPER); }

  inline GLFWkeyfun setKeyCallback(GLFWkeyfun iCallback) { return std::exchange(fKeyCallback, iCallback); }
  inline GLFWcharfun setCharCallback(GLFWcharfun iCallback) { return std::exchange(fCharCallback, iCallback); }

  void setInputModeLockKeyMods(bool iValue) { fInputModeLockKeyMods = iValue; }
  int computeModifierBits() const;

  bool onKeyDown(GLFWwindow *iWindow, Event const &iEvent);
  bool onKeyUp(GLFWwindow *iWindow, Event const &iEvent);
  void resetAllKeys(GLFWwindow *iWindow);
  void resetKey(GLFWwindow *iWindow, glfw_key_t iKey, int modifierBits);
  void resetKeysOnSuperRelease(GLFWwindow *iWindow);
  inline bool hasSuperPlusKeys() const { return !fSuperPlusKeys.empty(); }
  void handleSuperPlusKeys(GLFWwindow *iWindow, int iTimeout);

  void setStickyKeys(bool iStickyKeys);
  bool getStickyKeys() const { return fStickyKeys; };

public:
  static constexpr glfw_scancode_t getKeyScancode(glfw_key_t iKey) { return keyboard::keyCodeToScancode(iKey); }
  static const char* getKeyName(glfw_key_t iKey, glfw_scancode_t iScancode);

private:
  static constexpr glfw_key_state_t kStickyPress = 3;
  static constexpr glfw_key_t getGLFWKey(glfw_scancode_t iScancode) { return keyboard::scancodeToKeyCode(iScancode); }
  static constexpr glfw_scancode_t getKeyScancode(char const *iKeyboardEventCode) { return keyboard::keyboardEventCodeToScancode(iKeyboardEventCode); }

  // constexpr when we know that the key is a valid key
  constexpr glfw_key_state_t getValidKeyState(glfw_key_t iKey) const { return fKeyStates[iKey]; }
  constexpr bool isValidKeyPressed(glfw_key_t iKey) const { return getValidKeyState(iKey) != GLFW_RELEASE; }
  static constexpr bool isSpecialKey(glfw_key_t iKey) {
    return iKey == GLFW_KEY_LEFT_SHIFT   || iKey == GLFW_KEY_RIGHT_SHIFT   ||
           iKey == GLFW_KEY_LEFT_CONTROL || iKey == GLFW_KEY_RIGHT_CONTROL ||
           iKey == GLFW_KEY_LEFT_ALT     || iKey == GLFW_KEY_RIGHT_ALT     ||
           iKey == GLFW_KEY_LEFT_SUPER   || iKey == GLFW_KEY_RIGHT_SUPER;
  }

private:
  std::array<glfw_key_state_t, GLFW_KEY_LAST + 1> fKeyStates{GLFW_RELEASE};
  std::map<glfw_key_t, int> fSuperPlusKeys{};
  bool fInputModeLockKeyMods{};
  bool fStickyKeys{};
  GLFWkeyfun fKeyCallback{};
  GLFWcharfun fCharCallback{};
};


}

#endif //EMSCRIPTEN_GLFW_KEYBOARD_H
