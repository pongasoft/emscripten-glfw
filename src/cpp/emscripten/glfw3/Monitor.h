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

#ifndef EMSCRIPTEN_GLFW_MONITOR_H
#define EMSCRIPTEN_GLFW_MONITOR_H

#include <GLFW/glfw3.h>
#include "Object.h"

namespace emscripten::glfw3 {

class Monitor : public Object<GLFWmonitor>
{
public:
  // user pointer
  inline void *getUserPointer() const { return fUserPointer; }
  inline void setUserPointer(void *iPointer) { fUserPointer = iPointer; }
  char const *getName() const { return "Browser"; }

private:
  void *fUserPointer{};
};

}

#endif //EMSCRIPTEN_GLFW_MONITOR_H
