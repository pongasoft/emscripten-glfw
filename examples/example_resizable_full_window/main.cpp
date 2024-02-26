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
 * The purpose of this example is to demonstrate how to make the canvas resizable and occupy the full window. */

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
  const text = `${w}x${h} | ${mx}x${my} | CTRL+Q to terminate ${isFullscreen ? "" : '| CTRL+F for fullscreen'}`;
  ctx.font = '15px monospace';
  ctx.fillStyle = `rgb(${255 - color}, 0, 0)`;
  ctx.fillText(text, 10 + color, 20 + color);
})

//! Called for each frame
void renderFrame(GLFWwindow *iWindow)
{
  static int frameCount = 0;

  // poll events
  glfwPollEvents();

  int w,h; glfwGetWindowSize(iWindow, &w, &h);
  int fw,fh; glfwGetFramebufferSize(iWindow, &fw, &fh);
  double mx,my; glfwGetCursorPos(iWindow, &mx, &my);
  auto color = 127.0f + 127.0f * std::sin((float) frameCount++ / 120.f);
  jsRenderFrame(iWindow, w, h, fw, fh, mx, my, (int) color, emscripten_glfw_is_window_fullscreen(iWindow));
}

//! The main loop (called by emscripten for each frame)
void main_loop(void *iUserData)
{
  if(auto window = reinterpret_cast<GLFWwindow *>(iUserData); !glfwWindowShouldClose(window))
  {
    // not done => renderFrame
    renderFrame(window);
  }
  else
  {
    // done => terminating
    glfwTerminate();
    emscripten_cancel_main_loop();
  }
}

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

//! main
int main()
{
  // set a callback for errors otherwise if there is a problem, we won't know
  glfwSetErrorCallback(consoleErrorHandler);

  // print the version on the console
  printf("%s\n", glfwGetVersionString());

  // initialize the library
  if(!glfwInit())
    return -1;

  // no OpenGL (use canvas2D)
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // setting the association window <-> canvas
  emscripten_glfw_set_next_window_canvas_selector("#canvas");

  // create the only window
  auto window = glfwCreateWindow(320, 200, "Resizable Full Window | emscripten-glfw", nullptr, nullptr);
  if(!window)
    return -1;

  // makes the canvas resizable and match the full window size
  emscripten_glfw_make_canvas_resizable(window, "window", nullptr);

  // set callback for exit (CTRL+Q) and fullscreen (CTRL+F)
  glfwSetKeyCallback(window, onKeyChange);

  // tell emscripten to use "main_loop" as the main loop (window is user data)
  emscripten_set_main_loop_arg(main_loop, window, 0, GLFW_FALSE);
}
