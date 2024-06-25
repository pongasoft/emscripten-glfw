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
#include "Config.h"
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
glfw_key_state_t Keyboard::getKeyState(glfw_key_t iKey)
{
  if(iKey < 0 || iKey > GLFW_KEY_LAST)
  {
    ErrorHandler::instance().logError(GLFW_INVALID_VALUE, "Invalid key [%d]", iKey);
    return GLFW_RELEASE;
  }

  if(fKeyStates[iKey] == kStickyPress)
  {
    fKeyStates[iKey] = GLFW_RELEASE;
    return GLFW_PRESS;
  }

  return fKeyStates[iKey];
}

//------------------------------------------------------------------------
// Window::computeCallbackModifierBits
//------------------------------------------------------------------------
int Keyboard::computeCallbackModifierBits(EmscriptenKeyboardEvent const *iKeyboardEvent) const
{
  int bits = 0;
  if(iKeyboardEvent ? toCBool(iKeyboardEvent->shiftKey) : isShiftPressed())
    bits |= GLFW_MOD_SHIFT;
  if(iKeyboardEvent ? toCBool(iKeyboardEvent->ctrlKey) : isControlPressed())
    bits |= GLFW_MOD_CONTROL;
  if(iKeyboardEvent ? toCBool(iKeyboardEvent->altKey) : isAltPressed())
    bits |= GLFW_MOD_ALT;
  if(iKeyboardEvent ? toCBool(iKeyboardEvent->metaKey) : isSuperPressed())
    bits |= GLFW_MOD_SUPER;

  // TODO need to implement my own event callback... to call e.getModifierState("CapsLock") and e.getModifierState("NumLock")
//  if(iKeyboardEvent && fInputModeLockKeyMods)
//  {
//    if(toCBool(iKeyboardEvent->capsLockKey))
//      bits |= GLFW_MOD_CAPS_LOCK;
//    if(toCBool(iKeyboardEvent->numLockKey))
//      bits |= GLFW_MOD_NUM_LOCK;
//  }
  return bits;
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
    fKeyStates[key] = GLFW_PRESS;

    if(fKeyCallback)
      fKeyCallback(iWindow, key, scancode, iKeyboardEvent->repeat == GLFW_TRUE ? GLFW_REPEAT : GLFW_PRESS, computeCallbackModifierBits(iKeyboardEvent));
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

  const glfw_key_state_t state = GLFW_RELEASE;

  bool wasSuperPressed = isSuperPressed();

  if(fKeyStates[key] != GLFW_RELEASE && fKeyStates[key] != kStickyPress)
  {
    fKeyStates[key] = fStickyKeys ? kStickyPress : state;

    if(fKeyCallback)
      fKeyCallback(iWindow, key, scancode, state, computeCallbackModifierBits(iKeyboardEvent));
  }

  if(wasSuperPressed && !isSuperPressed())
    resetKeysOnSuperRelease(iWindow);

  return true;
}

//------------------------------------------------------------------------
// Keyboard::resetAllKeys
//------------------------------------------------------------------------
void Keyboard::resetAllKeys(GLFWwindow *iWindow)
{
  auto modifiedBits = computeCallbackModifierBits();

  for(auto key = 0; key < fKeyStates.size(); key++)
  {
    if(fKeyStates[key] != GLFW_RELEASE && fKeyStates[key] != kStickyPress)
    {
      fKeyStates[key] = GLFW_RELEASE;

      if(fKeyCallback)
        fKeyCallback(iWindow, key, getKeyScancode(key), GLFW_RELEASE, modifiedBits);
    }
  }
}

//------------------------------------------------------------------------
// Keyboard::setStickyKeys
//------------------------------------------------------------------------
void Keyboard::setStickyKeys(bool iStickyKeys)
{
  fStickyKeys = iStickyKeys;
  if(!fStickyKeys)
  {
    for(auto &state: fKeyStates)
    {
      if(state == kStickyPress)
        state = GLFW_RELEASE;
    }
  }
}

//------------------------------------------------------------------------
// Keyboard::resetKeysOnSuperRelease
//------------------------------------------------------------------------
void Keyboard::resetKeysOnSuperRelease(GLFWwindow *iWindow)
{
  auto modifiedBits = computeCallbackModifierBits();

  for(auto key = 0; key < fKeyStates.size(); key++)
  {
    if(key == GLFW_KEY_LEFT_SHIFT   || key == GLFW_KEY_RIGHT_SHIFT   ||
       key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL ||
       key == GLFW_KEY_LEFT_ALT     || key == GLFW_KEY_RIGHT_ALT     ||
       key == GLFW_KEY_LEFT_SUPER   || key == GLFW_KEY_RIGHT_SUPER)
    {
      // the special keys properly receive key up events...
      continue;
    }

    if(fKeyStates[key] != GLFW_RELEASE && fKeyStates[key] != kStickyPress)
    {
      fKeyStates[key] = GLFW_RELEASE;

      if(fKeyCallback)
        fKeyCallback(iWindow, key, getKeyScancode(key), GLFW_RELEASE, modifiedBits);
    }
  }
}

}