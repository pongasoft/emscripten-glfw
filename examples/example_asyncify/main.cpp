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

/**
 * The purpose of this example is to demonstrate how to use asyncify which allows the code to be written like you
 * would for a normal desktop application. */

//! Display error message in the Console
void consoleErrorHandler(int iErrorCode, char const *iErrorMessage)
{
  printf("glfwError: %d | %s\n", iErrorCode, iErrorMessage);
}

//! jsRenderFrame: for the sake of this example, uses the canvas2D api to change the color of the screen / display a message
EM_JS(void, jsRenderFrame, (GLFWwindow *glfwWindow, int w, int h, int fw, int fh, double mx, double my, int color, bool isFullscreen), {
  const ctx = Module.glfwGetCanvas(glfwWindow).getContext('2d');
  ctx.fillStyle = `rgb(${color}, ${color}, ${color})`;
  ctx.fillRect(0, 0, fw, fh); // using framebuffer width/height
  ctx.save();
  ctx.scale(fw/w, fh/h);
  const text = `${w}x${h} | ${mx}x${my} | CTRL+Q to terminate ${isFullscreen ? "" : '| CTRL+F for fullscreen'}`;
  ctx.font = '15px monospace';
  ctx.fillStyle = `rgb(${255 - color}, 0, 0)`;
  ctx.fillText(text, 10 + color, 20 + color);
  ctx.restore();
})

//! Handle key events => on CTRL+Q sets "window should close" flag, CTRL+F for fullscreen
void onKeyChange(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if(action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
    switch(key) {
      case GLFW_KEY_Q: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
      case GLFW_KEY_F: emscripten_glfw_request_fullscreen(window, false, true); break; // ok from a keyboard event
      default: break;
    }
  }
}

//! computeSleepTime
unsigned int computeSleepTime(unsigned int iFrameStart)
{
  // this commented code represents the real math accounting for timer frequency
//  constexpr auto kFrameDurationInSeconds = 1.0f / 60.f;
//  auto frameEnd = glfwGetTimerValue();
//  auto frameDurationInSeconds = static_cast<float>(frameEnd - iFrameStart) / static_cast<float>(glfwGetTimerFrequency());
//  auto remaining = kFrameDurationInSeconds - frameDurationInSeconds;
//  if(remaining < 0)
//    remaining = 0;
//  return static_cast<unsigned int>(remaining * 1000.f);

   // For the sake of this example
   return 16; // ~60fps as the work done is almost nothing
}

//! main
int main()
{
  // set a callback for errors otherwise if there is a problem, we won't know
  glfwSetErrorCallback(consoleErrorHandler);

  // initialize the library
  if(!glfwInit())
    return -1;

  // no OpenGL (use canvas2D)
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // Make hi dpi aware
  glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);

  // setting the association window <-> canvas
  emscripten_glfw_set_next_window_canvas_selector("#canvas");

  // create the only window
  auto window = glfwCreateWindow(600, 400, "Asyncify | emscripten-glfw", nullptr, nullptr);
  if(!window)
    return -1;

  // set callback for exit (CTRL+Q) and fullscreen (CTRL+F)
  glfwSetKeyCallback(window, onKeyChange);

  int frameCount = 0;

  while(!glfwWindowShouldClose(window))
  {
    auto frameStart = glfwGetTimerValue();

    glfwPollEvents();

    int w,h;
    glfwGetWindowSize(window, &w, &h);
    int fw,fh;
    glfwGetFramebufferSize(window, &fw, &fh);
    double mx,my;
    glfwGetCursorPos(window, &mx, &my);
    auto color = 127.0f + 127.0f * std::sin((float) frameCount++ / 120.f);
    jsRenderFrame(window, w, h, fw, fh, mx, my, (int) color, emscripten_glfw_is_window_fullscreen(window));

    // This is very important when using ASYNCIFY: you must sleep at least some otherwise it would be an
    // infinite loop and the UI would never refresh
    emscripten_sleep(computeSleepTime(frameStart));
  }

  glfwTerminate();
}
