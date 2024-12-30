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

#include <GLFW/emscripten_glfw3.h>
#include <emscripten/emscripten.h>
#include <cstdio>
#include <cmath>
#include <atomic>
#include <thread>
#include <sstream>

/**
 * The purpose of this example is to demonstrate how to use pthread.
 * It creates 2 threads that are incrementing a counter to simulate work on separate threads.
 * In total there are 4 threads:
 * - the main browser thread which executes `jsRenderFrame` and the key callback (`onKeyChange`)
 * - the `main` function which is its separate thread due to `-sPROXY_TO_PTHREAD` option
 * - two worker threads (the counters) started in `main`
 */

//! Display error message in the Console
void consoleErrorHandler(int iErrorCode, char const *iErrorMessage)
{
  printf("glfwError: %d | %s\n", iErrorCode, iErrorMessage);
}

/**
 * For the sake of this example, uses the canvas2D api to display a message
 * Because the canvas is accessed, this needs to run on the main thread => using MAIN_THREAD_EM_ASM API
 */
void jsRenderFrame(GLFWwindow *glfwWindow, int w, int h, int fw, int fh, double mx, double my, int color, char const *c1, char const *c2)
{
  MAIN_THREAD_EM_ASM(
    {
      const ctx = Module.glfwGetCanvas($0).getContext('2d');
      const w = $1; const h = $2; const fw = $3; const fh = $4;
      const color = $7;
      const c1 = UTF8ToString($8); const c2 = UTF8ToString($9);
      ctx.fillStyle = `rgb(${color}, ${color}, ${color})`;
      ctx.fillRect(0, 0, fw, fh); // using framebuffer width/height
      ctx.save();
      ctx.scale(fw/w, fh/h);
      const text = `${w}x${h} | ${$5}x${$6} | c1${c1} | c2${c2}`;
      ctx.font = '15px monospace';
      ctx.fillStyle = `rgb(${255 - color}, 0, 0)`;
      ctx.fillText(text, 10 + (color / 4), 20 + color);
      ctx.restore();
    }, glfwWindow, w, h, fw, fh, mx, my, color, c1, c2);
}

//! turns `std::this_thread::get_id()` into a string
static std::string computeThreadId(std::thread::id id = std::this_thread::get_id())
{
  std::stringstream ss;
  ss << id;
  return ss.str();
}


//! Handle key events => on CTRL+Q sets "window should close" flag, CTRL+F for fullscreen
void onKeyChange(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  printf("onKeyChange running on thread: %s\n", computeThreadId().c_str());

  if(action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
    switch(key) {
      case GLFW_KEY_Q: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
      case GLFW_KEY_F: emscripten_glfw_request_fullscreen(window, false, true); break; // ok from a keyboard event
      default: break;
    }
  }
}

/**
 * A counter that keeps track on which thread the counting is happening */
struct Counter
{
  unsigned long fValue{};
  std::thread::id fThreadId{};
  inline Counter inc() const { return {fValue + 1, std::this_thread::get_id()}; }
  std::string toString() const {
    std::stringstream ss;
    ss << "{" << fValue << ", " << fThreadId << "}";
    return ss.str();
  }
};

/**
 * In this example, main is NOT executed on the browser main thread due to -sPROXY_TO_PTHREAD option */
int main()
{
  // set a callback for errors otherwise if there is a problem, we won't know
  glfwSetErrorCallback(consoleErrorHandler);

  // print the version on the console
  printf("%s\n", glfwGetVersionString());

  printf("main running on thread: %s\n", computeThreadId().c_str());

  // initialize the library
  if(!glfwInit())
    return -1;

  // no OpenGL (use canvas2D)
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // setting the association window <-> canvas
  emscripten_glfw_set_next_window_canvas_selector("#canvas");

  // create the only window
  auto window = glfwCreateWindow(600, 400, "example_pthread | emscripten-glfw", nullptr, nullptr);
  if(!window)
    return -1;

  // set callback for exit (CTRL+Q) and fullscreen (CTRL+F)
  glfwSetKeyCallback(window, onKeyChange);

  std::atomic<bool> running{true};
  std::atomic<Counter> c1{};
  std::atomic<Counter> c2{};

  std::thread t1([&c1, &running] {
    printf("t1 running on thread: %s\n", computeThreadId().c_str());
    while(running) {
      c1.store(c1.load().inc());
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    printf("t1 done on thread: %s\n", computeThreadId().c_str());
  });

  std::thread t2([&c2, &running] {
    printf("t2 running on thread: %s\n", computeThreadId().c_str());
    while(running) {
      c2.store(c2.load().inc());
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    printf("t2 done on thread: %s\n", computeThreadId().c_str());
  });

  int frameCount = 0;

  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    int w,h;
    glfwGetWindowSize(window, &w, &h);
    int fw,fh;
    glfwGetFramebufferSize(window, &fw, &fh);
    double mx,my;
    glfwGetCursorPos(window, &mx, &my);
    auto color = 127.0f + 127.0f * std::sin((float) frameCount++ / 120.f);
    jsRenderFrame(window, w, h, fw, fh, mx, my, static_cast<int>(color), c1.load().toString().c_str(), c2.load().toString().c_str());

    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  running = false;

  glfwTerminate();

  t1.join();
  t2.join();
}
