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
// Keyboard::computeModifierBits
//------------------------------------------------------------------------
int Keyboard::computeModifierBits() const
{
  int bits = 0;
  if(isShiftPressed())
    bits |= GLFW_MOD_SHIFT;
  if(isControlPressed())
    bits |= GLFW_MOD_CONTROL;
  if(isAltPressed())
    bits |= GLFW_MOD_ALT;
  if(isSuperPressed())
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
bool Keyboard::onKeyDown(GLFWwindow *iWindow, Event const &iEvent, emscripten::glfw3::browser_key_fun_t const &iBrowserKeyCallback)
{
  auto scancode = getKeyScancode(iEvent.code);
  glfw_key_t key = getGLFWKey(scancode);

  // handling Super
  if(iEvent.isSuperPressed())
  {
    if(handleSuperKeyPressed(key, iEvent.repeat))
      return true;
  }

  auto handled = false;

  if(key != GLFW_KEY_UNKNOWN)
  {
    fKeyStates[key] = GLFW_PRESS;

    if(fKeyCallback)
      fKeyCallback(iWindow, key, scancode, iEvent.repeat ? GLFW_REPEAT : GLFW_PRESS, iEvent.modifierBits);

    handled = !iBrowserKeyCallback ||
              !iBrowserKeyCallback(iWindow, key, scancode, iEvent.repeat ? GLFW_REPEAT : GLFW_PRESS, iEvent.modifierBits);
  }

  if(fCharCallback)
  {
    if(iEvent.codepoint > 0)
      fCharCallback(iWindow, iEvent.codepoint);
  }

  return handled;
}

//------------------------------------------------------------------------
// Keyboard::onKeyUp
//------------------------------------------------------------------------
bool Keyboard::onKeyUp(GLFWwindow *iWindow, Event const &iEvent, emscripten::glfw3::browser_key_fun_t const &iBrowserKeyCallback)
{
  auto scancode = getKeyScancode(iEvent.code);
  glfw_key_t key = getGLFWKey(scancode);

  if(key == GLFW_KEY_UNKNOWN)
    return false;

  fSuperPlusKeys.erase(key);

  const glfw_key_state_t state = GLFW_RELEASE;

  bool wasSuperPressed = isSuperPressed();

  auto handled = false;

  if(fKeyStates[key] != GLFW_RELEASE && fKeyStates[key] != kStickyPress)
  {
    fKeyStates[key] = fStickyKeys ? kStickyPress : state;

    if(fKeyCallback)
      fKeyCallback(iWindow, key, scancode, state, iEvent.modifierBits);

    handled = !iBrowserKeyCallback ||
              !iBrowserKeyCallback(iWindow, key, scancode, state, iEvent.modifierBits);

  }

  if(wasSuperPressed && !isSuperPressed())
    resetKeysOnSuperRelease(iWindow);

  return handled;
}


//------------------------------------------------------------------------
// Keyboard::resetKey
//------------------------------------------------------------------------
void Keyboard::resetKey(GLFWwindow *iWindow, glfw_key_t iKey, int modifierBits)
{
  if(fKeyStates[iKey] != GLFW_RELEASE && fKeyStates[iKey] != kStickyPress)
  {
    fKeyStates[iKey] = GLFW_RELEASE;

    if(fKeyCallback)
      fKeyCallback(iWindow, iKey, getKeyScancode(iKey), GLFW_RELEASE, modifierBits);
  }
}

//------------------------------------------------------------------------
// Keyboard::resetAllKeys
//------------------------------------------------------------------------
void Keyboard::resetAllKeys(GLFWwindow *iWindow)
{
  for(auto key = 0; key < fKeyStates.size(); key++)
  {
    resetKey(iWindow, key, 0);
  }
  fSuperPlusKeys.clear();
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
// Keyboard::handleSuperKeyPressed
// Called when the super key is involved in a key down event (`e.metaKey` is `true`)
//------------------------------------------------------------------------
bool Keyboard::handleSuperKeyPressed(glfw_key_t iKey, bool iRepeat)
{
  // case Super + Key (Special keys behave differently, so we ignore them)
  if(!isSpecialKey(iKey))
  {
    // the issue is that the Up event is NEVER received... so we work around in multiple ways
    if(fSuperPlusKeys.find(iKey) == fSuperPlusKeys.end() && iRepeat)
    {
      // case when we have issued a key up event due to timeout... so we are ignoring
      return true;
    }
    // we store the current time (processed in handleSuperPlusKeys)
    fSuperPlusKeys[iKey] = {static_cast<int>(emscripten_get_now()), iRepeat};
  }

  // case when it is the super key itself: some keys might already be down
  if(isSuperKey(iKey) && !iRepeat)
  {
    for(int k = 0; k < fKeyStates.size(); k++)
    {
      if(!isSpecialKey(k) && fKeyStates[k] != GLFW_RELEASE)
      {
        fSuperPlusKeys[k] = {static_cast<int>(emscripten_get_now()), false};
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------
// Keyboard::resetKeysOnSuperRelease
// Called when the Super key has been released to reset all keys that were down
//------------------------------------------------------------------------
void Keyboard::resetKeysOnSuperRelease(GLFWwindow *iWindow)
{
  auto modifierBits = computeModifierBits();

  for(auto &[key, _]: fSuperPlusKeys)
  {
    resetKey(iWindow, key, modifierBits);
  }

  fSuperPlusKeys.clear();
}

//------------------------------------------------------------------------
// Keyboard::handleSuperPlusKeys
// Called every frame when fSuperPlusKeys is not empty to purge keys that have timed out
//------------------------------------------------------------------------
void Keyboard::handleSuperPlusKeys(GLFWwindow *iWindow, SuperPlusKeyTimeout const &iTimeout)
{
  auto modifierBits = computeModifierBits();

  auto now = static_cast<int>(emscripten_get_now());

  for(auto it = fSuperPlusKeys.begin(); it != fSuperPlusKeys.end();)
  {
    auto timeout = it->second.fRepeat ? iTimeout.fRepeatTimeout : iTimeout.fTimeout;
    if(it->second.fLastTimePressed + timeout <= now)
    {
      resetKey(iWindow, it->first, modifierBits);
      it = fSuperPlusKeys.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

}