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

#ifndef EMSCRIPTEN_GLFW_CURSOR_H
#define EMSCRIPTEN_GLFW_CURSOR_H

#include <GLFW/glfw3.h>
#include <array>
#include <memory>
#include "Object.h"

namespace emscripten::glfw3 {

using glfw_cursor_shape_t = int;

class Cursor : public Object<GLFWcursor>
{
public:
  constexpr Cursor(glfw_cursor_shape_t iShape, char const *iCSSValue) : fShape{iShape}, fCSSValue{iCSSValue} {}

  static constexpr Cursor const *findCursor(glfw_cursor_shape_t iShape)
  {
    auto i = std::find_if(kCursors.begin(), kCursors.end(), [iShape](auto &c) { return c.fShape == iShape; });
    return i == kCursors.end() ? nullptr : i;
  }

  static constexpr Cursor const *findCursor(GLFWcursor *iCursor)
  {
    if(!iCursor)
      return getDefault();

    auto i = std::find_if(kCursors.begin(), kCursors.end(), [iCursor](auto &c) { return c.asOpaquePtr() == iCursor; });
    return i == kCursors.end() ? nullptr : i;
  }

  static constexpr Cursor const *getDefault() { return findCursor(GLFW_ARROW_CURSOR); }
  static constexpr Cursor const *getHiddenCursor() { return &kCursorHidden; }

private:
  static const Cursor kCursorHidden;

public:
  glfw_cursor_shape_t fShape{};
  char const *fCSSValue{};

public:
  static const std::array<Cursor, 10> kCursors;
};

}

#endif //EMSCRIPTEN_GLFW_CURSOR_H
