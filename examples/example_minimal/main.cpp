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

#include <GLFW/glfw3.h>
#include <cstdio>
#include <emscripten/version.h>

/**
 * The purpose of this example is to be as minimal as possible: initializes glfw, create window, then destroy it
 * and terminate glfw. Uses the default shell that comes with emscripten. */

int main()
{
  // print the version on the console
  printf("%s\n", glfwGetVersionString());
  printf("emscripten: v%d.%d.%d\n", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);

  if(!glfwInit())
    return -1;

  auto window = glfwCreateWindow(320, 200, "example_minimal | emscripten-glfw", nullptr, nullptr);
  glfwDestroyWindow(window);

  glfwTerminate();
}
