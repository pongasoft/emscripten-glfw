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

#include <GLFW/emscripten_glfw3.h>
#include <cstdio>
#include <emscripten/html5.h>
#include "Triangle.h"
#include <map>

/**
 * This program acts both as a demo and as a test of many of the features of the implementation. As a result it is
 * probably not a good starting point. Please check the `examples` folder for more concise examples. */

static void consoleErrorHandler(int iErrorCode, char const *iErrorMessage)
{
  printf("glfwError: %d | %s\n", iErrorCode, iErrorMessage);
}

std::vector<std::shared_ptr<Triangle>> kTriangles{};


struct Event
{
  enum class Type : int
  {
    exit = 1,
    toggleShow,
    zoomIn,
    zoomOut,
    zoomReset,
    opacity30,
    opacity60,
    opacity100,
    close,
    toggleHiDPIAware,
    toggleResizable,
    toggleSizeLimits,
    toggleAspectRatio,
    updateTitle
  };

  Type fType;
  GLFWwindow *fWindow{};
};

//------------------------------------------------------------------------
// popEvent
//------------------------------------------------------------------------
std::optional<Event> popEvent()
{
  int type{};
  GLFWwindow *w;
  EM_ASM({
           if(Module.hasEvents()) {
             const event = Module.popEvent();
             setValue($0, event.type, 'i32');
             setValue($1, event.canvas, '*');
           }
         }, &type, &w);
  if(type > 0)
    return Event{static_cast<Event::Type>(type), w};
  else
    return std::nullopt;
}

//------------------------------------------------------------------------
// handleEvents
//------------------------------------------------------------------------
bool handleEvents()
{
  while(true)
  {
    auto event = popEvent();
    if(!event)
      return true;

    auto triangle = event->fWindow ?
                    *std::find_if(kTriangles.begin(), kTriangles.end(), [w = event->fWindow](auto const &triangle) { return triangle->getWindow() == w; }):
                    nullptr;

    switch(event->fType)
    {
      case Event::Type::exit: return false;
      case Event::Type::toggleShow: if(triangle) triangle->toggleShow(); break;
      case Event::Type::zoomIn: if(triangle) triangle->zoomIn(); break;
      case Event::Type::zoomOut: if(triangle) triangle->zoomOut(); break;
      case Event::Type::zoomReset: if(triangle) triangle->zoomReset(); break;
      case Event::Type::opacity30: if(triangle) triangle->setOpacity(0.3f); break;
      case Event::Type::opacity60: if(triangle) triangle->setOpacity(0.6f); break;
      case Event::Type::opacity100: if(triangle) triangle->setOpacity(1.0f); break;
      case Event::Type::close: if(triangle) triangle->close(); break;
      case Event::Type::toggleHiDPIAware: if(triangle) triangle->toggleHiDPIAware(); break;
      case Event::Type::toggleResizable: if(triangle) triangle->toggleResizable(); break;
      case Event::Type::toggleSizeLimits: if(triangle) triangle->toggleSizeLimits(); break;
      case Event::Type::toggleAspectRatio: if(triangle) triangle->toggleAspectRatio(); break;
      case Event::Type::updateTitle: if(triangle) triangle->updateTitle(); break;
      default: break;
    }
  }
}

//------------------------------------------------------------------------
// one iteration of the loop
//------------------------------------------------------------------------
bool iter()
{
  glfwPollEvents();

  if(!handleEvents())
    return false;

  for(auto it = kTriangles.begin(); it != kTriangles.end();)
  {
    if((*it)->shouldClose())
      it = kTriangles.erase(it);
    else
      ++it;
  }

  for(auto &v: kTriangles)
  {
    if(!v->render())
      return false;
  }

  for(auto &v: kTriangles)
    v->updateValues();

  Triangle::updateNoWindowValues();

  return !kTriangles.empty();
}

//------------------------------------------------------------------------
// loop
//------------------------------------------------------------------------
void loop()
{
  if(!iter())
  {
    kTriangles.clear();
    glfwTerminate();
    emscripten_cancel_main_loop();
  }
}

//------------------------------------------------------------------------
// main
//------------------------------------------------------------------------
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
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    emscripten_glfw_set_next_window_canvas_selector("#canvas1");
    window1 = glfwCreateWindow(300, 200, "hello world | canvas 1", nullptr, nullptr);
    if(!window1)
    {
      glfwTerminate();
      return -1;
    }
  }

  GLFWwindow *window2{};
  if(canvas2Enabled)
  {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    emscripten_glfw_set_next_window_canvas_selector("#canvas2");
    window2 = glfwCreateWindow(300, 200, "hello world | canvas 2", nullptr, nullptr);
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
    std::shared_ptr<Triangle> window2Triangle = Triangle::init(window2, "canvas2", "#canvas2-container", "#canvas2-handle");
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

  emscripten_set_main_loop(loop, 0, GLFW_FALSE);
}