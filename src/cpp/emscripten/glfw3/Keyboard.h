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

namespace emscripten::glfw3 {

using glfw_key_state_t = int; // ex: GLFW_RELEASE
using glfw_key_t = int; // ex: GLFW_KEY_A

class Keyboard
{
public:
  glfw_key_state_t getKeyState(glfw_key_t iKey) const;
  inline GLFWkeyfun setKeyCallback(GLFWkeyfun iCallback) { return std::exchange(fKeyCallback, iCallback); }
  inline GLFWcharfun setCharCallback(GLFWcharfun iCallback) { return std::exchange(fCharCallback, iCallback); }

  bool onKeyDown(GLFWwindow *iWindow, const EmscriptenKeyboardEvent *iKeyboardEvent);
  bool onKeyUp(GLFWwindow *iWindow, const EmscriptenKeyboardEvent *iKeyboardEvent);
  void resetAllKeys(GLFWwindow *iWindow);

public:
  static int getKeyScancode(glfw_key_t iKey);
  static const char* getKeyName(glfw_key_t iKey, int iScancode);

private:
  static glfw_key_t getGLFWKey(int iScancode);

private:
  std::array<glfw_key_state_t, GLFW_KEY_LAST + 1> fKeyStates{GLFW_RELEASE};
  GLFWkeyfun fKeyCallback{};
  GLFWcharfun fCharCallback{};
};


}

#endif //EMSCRIPTEN_GLFW_KEYBOARD_H
