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

#ifndef EMSCRIPTEN_GLFW_JOYSTICK_H
#define EMSCRIPTEN_GLFW_JOYSTICK_H

#include <GLFW/glfw3.h>
#include <array>
#include <string>
#include <emscripten/html5.h>

namespace emscripten::glfw3 {

using glfw_joystick_id_t = int;
using glfw_joystick_button_state_t = unsigned char;

class Joystick
{
public:
  explicit Joystick(glfw_joystick_id_t id) noexcept : fId{id} {}

  constexpr bool isPresent() const { return fPresent; }
  inline bool isGamepad() const { return fPresent && fMapping == "standard"; }
  void *getUserPointer() const { return fUserPointer; }
  void setUserPointer(void* userPointer) { fUserPointer = userPointer; }
  char const *getName() const { return fPresent ? fName.data() : nullptr; }
  char const *getMapping() const { return fPresent ? fMapping.data() : nullptr; }
  float const *getAxes(int *oCount) const;
  glfw_joystick_button_state_t const *getDigitalButtons(int *oCount) const;
  glfw_joystick_button_state_t const *getHats(int *oCount) const;
  int getGamepadState(GLFWgamepadstate *oState) const;

  static bool checkValidJoystick(glfw_joystick_id_t id);
  static Joystick &getJoystick(glfw_joystick_id_t id) { return kJoysticks.at(id); }
  static Joystick *findJoystick(glfw_joystick_id_t id) { return checkValidJoystick(id) ? &kJoysticks[id] : nullptr; }

  friend class Context;

private:
  void connect(EmscriptenGamepadEvent const *iEvent);
  void disconnect(EmscriptenGamepadEvent const *iEvent);
  void populate(EmscriptenGamepadEvent const *iEvent);

  static int computePresentJoystickCount();
  static int pollJoysticks();

private:
  static std::array<Joystick, GLFW_JOYSTICK_LAST + 1> kJoysticks;

  // Javascript gamepad API has 4 axes and 16 buttons
  // mapping is defined here https://w3c.github.io/gamepad/#remapping
  constexpr static size_t kNumGamepadAxes{4};
  constexpr static size_t kNumGamepadButtons{16};
  constexpr static auto kEmscriptenMaxNumDigitalButtons = std::extent_v<decltype(EmscriptenGamepadEvent::digitalButton)>;
  constexpr static auto kEmscriptenMaxNumAnalogButtons = std::extent_v<decltype(EmscriptenGamepadEvent::analogButton)>;
  constexpr static auto kEmscriptenMaxNumAxes = std::extent_v<decltype(EmscriptenGamepadEvent::axis)>;

private:
  glfw_joystick_id_t fId{};
  bool fPresent{};
  void *fUserPointer{};
  std::string fName{};
  std::string fMapping{};
  int fNumAxes{};
  int fNumButtons{};
  std::array<float, kEmscriptenMaxNumAxes> fAxes{};
  std::array<glfw_joystick_button_state_t, kEmscriptenMaxNumDigitalButtons> fDigitalButtons{};
  std::array<float, kEmscriptenMaxNumAnalogButtons> fAnalogButtons{};
  mutable glfw_joystick_button_state_t fDPad{};
};

}

#endif //EMSCRIPTEN_GLFW_JOYSTICK_H