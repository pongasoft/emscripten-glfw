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

namespace emscripten::glfw3 {

struct Monitor
{
  inline GLFWmonitor *asGLFWmonitor() { return reinterpret_cast<GLFWmonitor *>(this); }
  int fId;
};

}

#endif //EMSCRIPTEN_GLFW_MONITOR_H
