/*
 * Copyright (c) 2024 pongasoft
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

#include "Joystick.h"
#include "ErrorHandler.h"

namespace emscripten::glfw3 {

//------------------------------------------------------------------------
// Joystick::kJoysticks
//------------------------------------------------------------------------
std::array<Joystick, GLFW_JOYSTICK_LAST + 1> Joystick::kJoysticks{
  Joystick{GLFW_JOYSTICK_1},
  Joystick{GLFW_JOYSTICK_2},
  Joystick{GLFW_JOYSTICK_3},
  Joystick{GLFW_JOYSTICK_4},
  Joystick{GLFW_JOYSTICK_5},
  Joystick{GLFW_JOYSTICK_6},
  Joystick{GLFW_JOYSTICK_7},
  Joystick{GLFW_JOYSTICK_8},
  Joystick{GLFW_JOYSTICK_9},
  Joystick{GLFW_JOYSTICK_10},
  Joystick{GLFW_JOYSTICK_11},
  Joystick{GLFW_JOYSTICK_12},
  Joystick{GLFW_JOYSTICK_13},
  Joystick{GLFW_JOYSTICK_14},
  Joystick{GLFW_JOYSTICK_15},
  Joystick{GLFW_JOYSTICK_16}
};

//------------------------------------------------------------------------
// Joystick::checkValidJoystick
//------------------------------------------------------------------------
bool Joystick::checkValidJoystick(glfw_joystick_id_t id)
{
  if(id >= GLFW_JOYSTICK_1 && id <= GLFW_JOYSTICK_LAST)
    return true;

  ErrorHandler::instance().logError(GLFW_INVALID_VALUE, "invalid joystick id [%d]", id);
  return false;
}

//------------------------------------------------------------------------
// Joystick::connect
//------------------------------------------------------------------------
void Joystick::connect(EmscriptenGamepadEvent const *iEvent)
{
  fPresent = true;
  fName = iEvent->id;
  fMapping = iEvent->mapping;
}

//------------------------------------------------------------------------
// Joystick::disconnect
//------------------------------------------------------------------------
void Joystick::disconnect(EmscriptenGamepadEvent const *iEvent)
{
  fPresent = false;
  fName = {};
  fMapping = {};
}

//------------------------------------------------------------------------
// Joystick::populate
//------------------------------------------------------------------------
void Joystick::populate(EmscriptenGamepadEvent const *iEvent)
{
  fNumButtons = std::clamp(iEvent->numButtons, 0, static_cast<int>(fDigitalButtons.size()));
  for(auto i = 0; i < fNumButtons; i++)
  {
    fDigitalButtons[i] = iEvent->digitalButton[i];
  }

  fNumAxes = std::clamp(iEvent->numAxes, 0, static_cast<int>(fAxes.size()));
  for(auto i = 0; i < fNumAxes; i++)
  {
    fAxes[i] = static_cast<float>(iEvent->axis[i]);
  }
}

//------------------------------------------------------------------------
// Joystick::getAxes
//------------------------------------------------------------------------
float const *Joystick::getAxes(int *oCount) const
{
  if(fPresent)
  {
    *oCount = fNumAxes;
    return fAxes.data();
  }
  else
  {
    *oCount = 0;
    return nullptr;
  }
}

//------------------------------------------------------------------------
// Joystick::getDigitalButtons
//------------------------------------------------------------------------
glfw_joystick_button_state_t const *Joystick::getDigitalButtons(int *oCount) const
{
  if(fPresent)
  {
    *oCount = fNumButtons;
    return fDigitalButtons.data();
  }
  else
  {
    *oCount = 0;
    return nullptr;
  }
}

//------------------------------------------------------------------------
// Joystick::getHats
//------------------------------------------------------------------------
glfw_joystick_button_state_t const *Joystick::getHats(int *oCount) const
{
  // see https://w3c.github.io/gamepad/#remapping for DPad buttons
  if(isGamepad() && fNumButtons > 15)
  {
    *oCount = 1;
    fDPad = 0;
    if(fDigitalButtons[12]) fDPad |= GLFW_HAT_UP;
    if(fDigitalButtons[13]) fDPad |= GLFW_HAT_DOWN;
    if(fDigitalButtons[14]) fDPad |= GLFW_HAT_LEFT;
    if(fDigitalButtons[15]) fDPad |= GLFW_HAT_RIGHT;
    return &fDPad;
  }
  else
  {
    *oCount = 0;
    return nullptr;
  }
}

//------------------------------------------------------------------------
// Joystick::getGamepadState
//------------------------------------------------------------------------
int Joystick::getGamepadState(GLFWgamepadstate *oState) const
{
  if(isGamepad() && fNumAxes >= 4 && fNumButtons >= 16)
  {
    // axes
    oState->axes[GLFW_GAMEPAD_AXIS_LEFT_X] = fAxes[0];
    oState->axes[GLFW_GAMEPAD_AXIS_LEFT_Y] = fAxes[1];
    oState->axes[GLFW_GAMEPAD_AXIS_RIGHT_X] = fAxes[2];
    oState->axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] = fAxes[3];
    oState->axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]  = fDigitalButtons[10] ? 1.0f : 0;
    oState->axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = fDigitalButtons[11] ? 1.0f : 0;

    // buttons
    oState->buttons[GLFW_GAMEPAD_BUTTON_A] = fDigitalButtons[0];
    oState->buttons[GLFW_GAMEPAD_BUTTON_B] = fDigitalButtons[1];
    oState->buttons[GLFW_GAMEPAD_BUTTON_X] = fDigitalButtons[2];
    oState->buttons[GLFW_GAMEPAD_BUTTON_Y] = fDigitalButtons[3];
    oState->buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] = fDigitalButtons[4];
    oState->buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = fDigitalButtons[5];
    oState->buttons[GLFW_GAMEPAD_BUTTON_BACK] = fDigitalButtons[8];
    oState->buttons[GLFW_GAMEPAD_BUTTON_START] = fDigitalButtons[9];
    oState->buttons[GLFW_GAMEPAD_BUTTON_GUIDE] = fDigitalButtons[16];
    oState->buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] = fDigitalButtons[6];
    oState->buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] = fDigitalButtons[7];
    oState->buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] = fDigitalButtons[12];
    oState->buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = fDigitalButtons[15];
    oState->buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] = fDigitalButtons[13];
    oState->buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] = fDigitalButtons[14];

    return GLFW_TRUE;
  }
  else
    return GLFW_FALSE;
}

//------------------------------------------------------------------------
// Joystick::computePresentJoystickCount
//------------------------------------------------------------------------
int Joystick::computePresentJoystickCount()
{
  return std::count_if(kJoysticks.begin(), kJoysticks.end(), [](auto &j) { return j.fPresent; });
}

//------------------------------------------------------------------------
// Joystick::pollJoysticks
//------------------------------------------------------------------------
int Joystick::pollJoysticks()
{
  int count = 0;
  if(emscripten_sample_gamepad_data() == EMSCRIPTEN_RESULT_SUCCESS)
  {
    for(auto &j: kJoysticks)
    {
      if(j.isPresent())
      {
        count++;
        EmscriptenGamepadEvent data;
        if(emscripten_get_gamepad_status(j.fId, &data) == EMSCRIPTEN_RESULT_SUCCESS)
          j.populate(&data);
      }
    }
  }
  return count;
}



}