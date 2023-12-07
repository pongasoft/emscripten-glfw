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
#include <stdio.h>
#include <emscripten/html5.h>

static void consoleErrorHandler(int iErrorCode, char const *iErrorMessage)
{
  printf("glfwError: %d | %s\n", iErrorCode, iErrorMessage);
}

EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData) {
  printf("key_callback\n");
  glfwSetWindowShouldClose((GLFWwindow *) userData, GLFW_TRUE);
  return GLFW_TRUE;
}

void onContentScaleChange(GLFWwindow *iWindow, float xScale, float yScale)
{
  printf("onContentScaleChange: %fx%f\n", xScale, yScale);
}

int main()
{
  glfwSetErrorCallback(consoleErrorHandler);

  if(!glfwInit())
    return -1;

  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  GLFWwindow *window = glfwCreateWindow(600, 500, "hello world", NULL, NULL);
  if(!window)
  {
    glfwTerminate();
    return -1;
  }

  glfwSetWindowContentScaleCallback(window, onContentScaleChange);

  glfwMakeContextCurrent(window);

  emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, window, 1, key_callback);

  while(!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT);
    emscripten_sleep(100);
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}