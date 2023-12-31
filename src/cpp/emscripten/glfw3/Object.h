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

#ifndef EMSCRIPTEN_GLFW_OBJECT_H
#define EMSCRIPTEN_GLFW_OBJECT_H

namespace emscripten::glfw3 {

template<typename G>
class Object
{
public:
  using opaque_ptr_t = G *;

  virtual ~Object() = default;
  inline G *asOpaquePtr() { return reinterpret_cast<G *>(&fOpaquePtr); }
  // because G is opaque it is fine to return non const (nothing can happen)
  inline G *asOpaquePtr() const { return const_cast<G *>(reinterpret_cast<G const *>(&fOpaquePtr)); }
private:
  struct OpaquePtr {};
  OpaquePtr fOpaquePtr{};
};

}

#endif //EMSCRIPTEN_GLFW_OBJECT_H
