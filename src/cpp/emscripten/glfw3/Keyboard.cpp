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

#include "Keyboard.h"
#include "ErrorHandler.h"

extern "C" {
int emscripten_glfw3_context_to_codepoint(char const *);
}

namespace emscripten::glfw3 {

//------------------------------------------------------------------------
// Keyboard::getKeyName
//------------------------------------------------------------------------
const char *Keyboard::getKeyName(glfw_key_t iKey, glfw_scancode_t iScancode)
{
  auto scancode = iKey == GLFW_KEY_UNKNOWN ? iScancode : getKeyScancode(iKey);
  return keyboard::scancodeToString(scancode);
}

//------------------------------------------------------------------------
// Keyboard::getKeyState
//------------------------------------------------------------------------
glfw_key_state_t Keyboard::getKeyState(glfw_key_t iKey) const
{
  if(iKey < 0 || iKey > GLFW_KEY_LAST)
  {
    ErrorHandler::instance().logError(GLFW_INVALID_VALUE, "Invalid key [%d]", iKey);
    return GLFW_RELEASE;
  }
  return fKeyStates[iKey];
}

//------------------------------------------------------------------------
// Keyboard::onKeyDown
//------------------------------------------------------------------------
bool Keyboard::onKeyDown(GLFWwindow *iWindow, EmscriptenKeyboardEvent const *iKeyboardEvent)
{
  auto scancode = getKeyScancode(iKeyboardEvent->code);
  glfw_key_t key = getGLFWKey(scancode);

  if(key != GLFW_KEY_UNKNOWN)
  {
    glfw_key_state_t state = iKeyboardEvent->repeat == GLFW_TRUE ? GLFW_REPEAT : GLFW_PRESS;

    fKeyStates[key] = state;

    if(fKeyCallback)
      // TODO handle mods
      fKeyCallback(iWindow, key, scancode, state, 0);
  }

  if(fCharCallback) {
    if(auto codepoint = emscripten_glfw3_context_to_codepoint(iKeyboardEvent->key); codepoint > 0)
      fCharCallback(iWindow, codepoint);
  }

  return true;
}

//------------------------------------------------------------------------
// Keyboard::onKeyUp
//------------------------------------------------------------------------
bool Keyboard::onKeyUp(GLFWwindow *iWindow, EmscriptenKeyboardEvent const *iKeyboardEvent)
{
  auto scancode = getKeyScancode(iKeyboardEvent->code);
  glfw_key_t key = getGLFWKey(scancode);

  if(key == GLFW_KEY_UNKNOWN)
    return false;

  glfw_key_state_t state = GLFW_RELEASE;

  if(fKeyStates[key] != GLFW_RELEASE)
  {
    fKeyStates[key] = state;

    if(fKeyCallback)
      // TODO handle mods
      fKeyCallback(iWindow, key, scancode, state, 0);
  }

  return true;
}

//------------------------------------------------------------------------
// Keyboard::resetAllKeys
//------------------------------------------------------------------------
void Keyboard::resetAllKeys(GLFWwindow *iWindow)
{
  for(auto key = 0; key < fKeyStates.size(); key++)
  {
    if(fKeyStates[key] != GLFW_RELEASE)
    {
      fKeyStates[key] = GLFW_RELEASE;

      if(fKeyCallback)
        fKeyCallback(iWindow, key, getKeyScancode(key), GLFW_RELEASE, 0);
    }
  }
}

}