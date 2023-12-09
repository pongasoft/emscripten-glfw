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

#include <GLFW/glfw3.h>
#include <cstdio>
#include <emscripten/html5.h>
#include "Triangle.h"

static void consoleErrorHandler(int iErrorCode, char const *iErrorMessage)
{
  printf("glfwError: %d | %s\n", iErrorCode, iErrorMessage);
}

EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData) {
  printf("key_callback [%c]\n", e->keyCode);
  GLFWwindow *window = (GLFWwindow *) userData;
  switch(e->keyCode)
  {
    case ' ':
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
    case 'S':
    {
      int w,h;
      glfwGetWindowSize(window, &w, &h);
      glfwSetWindowSize(window, w * 2, h * 2);
      break;
    }
    case 's':
    {
      int w,h;
      glfwGetWindowSize(window, &w, &h);
      glfwSetWindowSize(window, w / 2, h / 2);
      break;
    }
    default:
      break;
  }
  return GLFW_TRUE;
}

void onContentScaleChange(GLFWwindow *iWindow, float xScale, float yScale)
{
  printf("onContentScaleChange: %fx%f\n", xScale, yScale);
}

void onWindowSizeChange(GLFWwindow* window, int width, int height)
{
  printf("onWindowSizeChange: %dx%d\n", width, height);
}

void onFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
  printf("onFramebufferSizeChange: %dx%d\n", width, height);
}

int main()
{
  glfwSetErrorCallback(consoleErrorHandler);

  if(!glfwInit())
    return -1;

  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  auto window = glfwCreateWindow(600, 500, "hello world", nullptr, nullptr);
  if(!window)
  {
    glfwTerminate();
    return -1;
  }

  glfwSetWindowContentScaleCallback(window, onContentScaleChange);
  glfwSetWindowSizeCallback(window, onWindowSizeChange);
  glfwSetFramebufferSizeCallback(window, onFramebufferSizeChange);

  auto window1Triangle = Triangle::init(window);
  if(!window1Triangle)
  {
    glfwTerminate();
    return -1;
  }

  window1Triangle->setBgColor(0.5f, 0.5f, 0.5f);

  glfwMakeContextCurrent(window);

  emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, window, 1, key_callback);

  while(!glfwWindowShouldClose(window))
  {
    if(!window1Triangle->render())
      break;

    // Render Step 5: Swap buffers - present back buffer, and prepare current
    // surface for next frame
//    glfwSwapBuffers(window);
//    glfwPollEvents();

    emscripten_sleep(100);
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}