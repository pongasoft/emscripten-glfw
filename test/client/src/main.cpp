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

#define GLFW_EMSCRIPTEN_CANVAS_SELECTOR  0x00027001

std::vector<std::shared_ptr<Triangle>> kTriangles{};

bool terminated() { return static_cast<bool>(EM_ASM_INT( return Module.terminated; )); }

int main()
{
  glfwSetErrorCallback(consoleErrorHandler);

  if(!glfwInit())
    return -1;

  auto canvas1Enabled = static_cast<bool>(EM_ASM_INT( return Module.canvas1Enabled; ));
  auto canvas2Enabled = static_cast<bool>(EM_ASM_INT( return Module.canvas2Enabled; ));

  GLFWwindow *window1{};
  if(canvas1Enabled)
  {
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHintString(GLFW_EMSCRIPTEN_CANVAS_SELECTOR, "#canvas1");
    window1 = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
    if(!window1)
    {
      glfwTerminate();
      return -1;
    }
  }

  GLFWwindow *window2{};
  if(canvas2Enabled)
  {
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    glfwWindowHintString(GLFW_EMSCRIPTEN_CANVAS_SELECTOR, "#canvas2");
    window2 = glfwCreateWindow(300, 200, "hello world", nullptr, nullptr);
    if(!window2)
    {
      glfwTerminate();
      return -1;
    }
  }

  if(window1)
  {
    std::shared_ptr<Triangle> window1Triangle = Triangle::init(window1, "canvas1");
    if(!window1Triangle)
    {
      glfwTerminate();
      return -1;
    }
    window1Triangle->setBgColor(0.5f, 0.5f, 0.5f);
    kTriangles.emplace_back(window1Triangle);
  }

  if(window2)
  {
    std::shared_ptr<Triangle> window2Triangle = Triangle::init(window2, "canvas2");
    if(!window2Triangle)
    {
      glfwTerminate();
      return -1;
    }
    window2Triangle->setBgColor(1.0f, 0, 0.5f);
    kTriangles.emplace_back(window2Triangle);
  }

  for(auto &v: kTriangles)
    v->registerCallbacks();

  if(window1 && window2)
    glfwFocusWindow(window1);

  Triangle::registerNoWindowCallbacks();

  while(!kTriangles.empty() && !terminated())
  {
    for(auto it = kTriangles.begin(); it != kTriangles.end();)
    {
      if((*it)->shouldClose())
        it = kTriangles.erase(it);
      else
        ++it;
    }
    bool exitWhile = false;
    for(auto &v: kTriangles)
      exitWhile |= !v->render();
    if(exitWhile)
      break;

    glfwPollEvents();

    for(auto &v: kTriangles)
      v->updateValues();

    Triangle::updateNoWindowValues();

    emscripten_sleep(33); // ~30 fps
    glfwPollEvents();
  }

  kTriangles.clear();

  if(window1)
    emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, window1, 1, nullptr);

  glfwTerminate();
}