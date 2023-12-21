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

#include <emscripten/dom_pk_codes.h>
#include "Keyboard.h"
#include "ErrorHandler.h"

namespace emscripten::glfw3 {

//------------------------------------------------------------------------
// Keyboard::getKeyScancode
//------------------------------------------------------------------------
int Keyboard::getKeyScancode(glfw_key_t iKey)
{
  switch(iKey)
  {
    case GLFW_KEY_A: return DOM_PK_A;
    case GLFW_KEY_Q: return DOM_PK_Q;
    case GLFW_KEY_TAB: return DOM_PK_TAB;
    default: return -1;
  }
}

//------------------------------------------------------------------------
// Keyboard::getGLFWKey
//------------------------------------------------------------------------
glfw_key_t Keyboard::getGLFWKey(int iScancode)
{
  switch(iScancode)
  {
    case DOM_PK_A: return GLFW_KEY_A;
    case DOM_PK_Q: return GLFW_KEY_Q;
    case DOM_PK_TAB: return GLFW_KEY_TAB;
    default: return GLFW_KEY_UNKNOWN;
  }
}

//------------------------------------------------------------------------
// Keyboard::getKeyName
//------------------------------------------------------------------------
const char *Keyboard::getKeyName(glfw_key_t iKey, int iScancode)
{
  auto scancode = iKey == GLFW_KEY_UNKNOWN ? iScancode : getKeyScancode(iKey);
  return emscripten_dom_pk_code_to_string(scancode);
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
  auto scancode = emscripten_compute_dom_pk_code(iKeyboardEvent->code);
  glfw_key_t key = getGLFWKey(scancode);

  if(key == GLFW_KEY_UNKNOWN)
    return false;

  glfw_key_state_t state = iKeyboardEvent->repeat == GLFW_TRUE ? GLFW_REPEAT : GLFW_PRESS;

  fKeyStates[key] = state;

  if(fKeyCallback)
    // TODO handle mods
    fKeyCallback(iWindow, key, scancode, state, 0);

  if(fCharCallback)
    // TODO charCode is deprecated and somehow need to use js String.fromCharCode(iKeyboardEvent->key)??? maybe???
    fCharCallback(iWindow, iKeyboardEvent->charCode);

  return true;
}

//------------------------------------------------------------------------
// Keyboard::onKeyUp
//------------------------------------------------------------------------
bool Keyboard::onKeyUp(GLFWwindow *iWindow, EmscriptenKeyboardEvent const *iKeyboardEvent)
{
  auto scancode = emscripten_compute_dom_pk_code(iKeyboardEvent->code);
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