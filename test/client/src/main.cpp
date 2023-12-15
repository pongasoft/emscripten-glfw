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
#include <map>

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

#define GLFW_EMSCRIPTEN_CANVAS_SELECTOR  0x00027001

std::map<GLFWwindow *, std::shared_ptr<Triangle>> kTriangles{};

int main()
{
  glfwSetErrorCallback(consoleErrorHandler);

  if(!glfwInit())
    return -1;

  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
  glfwWindowHintString(GLFW_EMSCRIPTEN_CANVAS_SELECTOR, "#canvas1");

  auto window1 = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
  if(!window1)
  {
    glfwTerminate();
    return -1;
  }

  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
  glfwWindowHintString(GLFW_EMSCRIPTEN_CANVAS_SELECTOR, "#canvas2");
  auto window2 = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
  if(!window2)
  {
    glfwTerminate();
    return -1;
  }

  {
    std::shared_ptr<Triangle> window1Triangle = Triangle::init(window1, "canvas1");
    if(!window1Triangle)
    {
      glfwTerminate();
      return -1;
    }
    window1Triangle->setBgColor(0.5f, 0.5f, 0.5f);
    kTriangles[window1] = window1Triangle;
  }

  {
    std::shared_ptr<Triangle> window2Triangle = Triangle::init(window2, "canvas2");
    if(!window2Triangle)
    {
      glfwTerminate();
      return -1;
    }
    window2Triangle->setBgColor(1.0f, 0, 0.5f);
    kTriangles[window2] = window2Triangle;
  }

  for(auto &[k, v]: kTriangles)
    v->registerCallbacks();

    emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, window1, 1, key_callback);

  while(true)
  {
    bool exitWhile = false;
    for(auto &[k, v]: kTriangles)
      exitWhile |= v->shouldClose();
    if(exitWhile)
      break;
    for(auto &[k, v]: kTriangles)
      exitWhile |= !v->render();
    if(exitWhile)
      break;

    glfwPollEvents();

    for(auto &[k, v]: kTriangles)
      v->updateValues();

    emscripten_sleep(33); // ~30 fps
  }

  kTriangles.clear();

  emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, window1, 1, nullptr);

  glfwTerminate();
}