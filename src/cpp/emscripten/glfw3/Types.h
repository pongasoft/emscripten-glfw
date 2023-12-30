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

#ifndef EMSCRIPTEN_GLFW_TYPES_H
#define EMSCRIPTEN_GLFW_TYPES_H

namespace emscripten::glfw3 {

template<typename T>
struct Vec2
{
  union
  {
    T x;
    T width;
  };

  union
  {
    T y;
    T height;
  };

  template<typename U>
  constexpr Vec2<T> &operator=(Vec2<U> const &iOther)
  {
    x = static_cast<T>(iOther);
    y = static_cast<T>(iOther);
    return *this;
  }

  constexpr bool operator==(Vec2 const &rhs) const { return x == rhs.x && y == rhs.y; }
  constexpr bool operator!=(Vec2 const &rhs) const { return !(rhs == *this); }
};

template<typename T>
static constexpr Vec2<T> operator+(const Vec2<T> &lhs, const Vec2<T> &rhs) { return {lhs.x + rhs.x, lhs.y + rhs.y}; }

template<typename T, typename U>
static constexpr Vec2<T> operator-(const Vec2<T> &lhs, const Vec2<U> &rhs) { return {lhs.x - rhs.x, lhs.y - rhs.y}; }

}

#endif //EMSCRIPTEN_GLFW_TYPES_H
