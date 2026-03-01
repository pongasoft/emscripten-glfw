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

void setHtmlValue(std::string_view iElementSelector, std::string_view iValue);

struct Event
{
  enum class Type : int
  {
    exit = 1,
    toggleShow = 2,
    zoomIn = 3,
    zoomOut = 4,
    zoomReset = 5,
    opacity = 6,
    close = 7,
    toggleHiDPIAware = 8,
    toggleResizable = 9,
    toggleSizeLimits = 10,
    toggleAspectRatio = 11,
    updateTitle = 12,
  };

  Type fType;
  GLFWwindow *fWindow{};
  float fData{};
};

//------------------------------------------------------------------------
// popEvent
//------------------------------------------------------------------------
std::optional<Event> popEvent()
{
  int type{};
  GLFWwindow *w;
  float data;
  EM_ASM({
           if(Module['hasEvents']()) {
             const event = Module['popEvent']();
             setValue($0, event.type, 'i32');
             setValue($1, event.canvas, '*');
             setValue($2, event.data, 'float');
           }
         }, &type, &w, &data);
  if(type > 0)
    return Event{static_cast<Event::Type>(type), w, data};
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
      case Event::Type::opacity: if(triangle) triangle->setOpacity(event->fData / 100.0f); break;
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
// debugEventHandlers
//------------------------------------------------------------------------
void debugEventHandlers(std::string_view iMessage)
{
  EM_ASM({
           const message = UTF8ToString($0);
           console.log(`${message} | Event Handlers = ${JSEvents.eventHandlers.length}`);
         },
         iMessage.data());
}

//------------------------------------------------------------------------
// applicationMouseMoveCallback
//------------------------------------------------------------------------
static bool applicationMouseMoveCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
  static long count = 0;
  count++;
  if(count % 500 == 0)
    printf("applicationMouseMoveCallback [%ld]\n", count);
  return false;
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
    debugEventHandlers("After glfwTerminate()");
    emscripten_html5_remove_event_listener(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EMSCRIPTEN_EVENT_MOUSEMOVE, reinterpret_cast<void *>(applicationMouseMoveCallback));
    debugEventHandlers("After removing application event listener");
    emscripten_cancel_main_loop();
  }
}

//------------------------------------------------------------------------
// main
//------------------------------------------------------------------------
int main()
{
  glfwSetErrorCallback(consoleErrorHandler);

  glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_EMSCRIPTEN);

  debugEventHandlers("Before glfwInit()");

  if(!glfwInit())
    return -1;

  debugEventHandlers("After glfwInit()");

  printf("GLFW: %s | Platform: 0x%x\n", glfwGetVersionString(), glfwGetPlatform());
  printf("emscripten: v%d.%d.%d\n", __EMSCRIPTEN_MAJOR__, __EMSCRIPTEN_MINOR__, __EMSCRIPTEN_TINY__);
  setHtmlValue("#version", glfwGetVersionString());
  setHtmlValue("#action-key", emscripten::glfw3::IsRuntimePlatformApple() ? "&#x2318" : "CTRL");

  auto canvas1Enabled = static_cast<bool>(EM_ASM_INT( return Module['canvas1Enabled']; ));
#ifndef EMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT
  auto canvas2Enabled = static_cast<bool>(EM_ASM_INT( return Module['canvas2Enabled']; ));
#else
  auto canvas2Enabled = false;
#endif

  GLFWwindow *window1{};
  if(canvas1Enabled)
  {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);
    emscripten_glfw_set_next_window_canvas_selector("#canvas1");
    window1 = glfwCreateWindow(300, 200, "hello world | canvas 1", nullptr, nullptr);
    if(!window1)
    {
      glfwTerminate();
      return -1;
    }
    setHtmlValue("input.canvas1.opacity", std::to_string(static_cast<int>(glfwGetWindowOpacity(window1) * 100.0f)));
    debugEventHandlers("After glfwCreateWindow(window1)");
  }

  GLFWwindow *window2{};
  if(canvas2Enabled)
  {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);
    emscripten_glfw_set_next_window_canvas_selector("#canvas2");
    window2 = glfwCreateWindow(300, 200, "hello world | canvas 2", nullptr, nullptr);
    if(!window2)
    {
      glfwTerminate();
      return -1;
    }
    setHtmlValue("input.canvas2.opacity", std::to_string(static_cast<int>(glfwGetWindowOpacity(window2) * 100.0f)));
    debugEventHandlers("After glfwCreateWindow(window2)");
  }

  if(window1)
  {
    std::shared_ptr<Triangle> window1Triangle = Triangle::init(window1, "canvas1");
    if(!window1Triangle)
    {
      glfwTerminate();
      return -1;
    }
    window1Triangle->setClickURL("https://github.com/pongasoft/emscripten-glfw");
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
    window2Triangle->setClickURL("https://github.com/pongasoft");
    window2Triangle->setBgColor(1.0f, 0, 0.5f);
    kTriangles.emplace_back(window2Triangle);
  }

  for(auto &v: kTriangles)
    v->registerCallbacks();

  if(window1 && window2)
    glfwFocusWindow(window1);

  Triangle::registerNoWindowCallbacks();

  // allow F12 to be handled by the browser
  emscripten::glfw3::AddBrowserKeyCallback([](GLFWwindow* window, int key, int scancode, int action, int mods) {
    return mods == 0 && action == GLFW_PRESS && key == GLFW_KEY_F12;
  });

  // Setting a callback that we know the library is also setting to make sure that when the library terminates
  // it does not remove the callbacks set by the application
  emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, false, applicationMouseMoveCallback);

  debugEventHandlers("After adding application event listener");

  emscripten_set_main_loop(loop, 0, false);
}