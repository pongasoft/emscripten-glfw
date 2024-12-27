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
#include <emscripten/html5_webgl.h>

#include <cstdio>
#include <cmath>

/**
 * The purpose of this example is to demonstrate the use of OffscreenCanvas which enables using WebGL contexts in pthreads */

//! Display error message in the Console
void consoleErrorHandler(int iErrorCode, char const *iErrorMessage)
{
  printf("glfwError: %d | %s\n", iErrorCode, iErrorMessage);
}

//! Called for each frame
void renderFrame(GLFWwindow *iWindow)
{
  static int frameCount = 0;

  // poll events
  glfwPollEvents();

  // int w,h; glfwGetWindowSize(iWindow, &w, &h);
  // int fw,fh; glfwGetFramebufferSize(iWindow, &fw, &fh);
  // double mx,my; glfwGetCursorPos(iWindow, &mx, &my);
  auto color = 127.0f + 127.0f * std::sin((float) frameCount++ / 120.f);

  color = color/255.0f;
  glClearColor(color, color, color, 1);
	glClear(GL_COLOR_BUFFER_BIT);
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
      case GLFW_KEY_F: emscripten::glfw3::RequestFullscreen(window, false, true); break; // ok from a keyboard event
      default: break;
    }
  }
}

//! main
int main()
{
  if (!emscripten_supports_offscreencanvas())
  {
    printf("This example requires OffscreenCanvas support\n");
    return -1;
  }

  // set a callback for errors otherwise if there is a problem, we won't know
  glfwSetErrorCallback(consoleErrorHandler);

  // print the version on the console
  printf("%s\n", glfwGetVersionString());

  // initialize the library
  if(!glfwInit())
    return -1;

  // setting the association window <-> canvas
  emscripten::glfw3::SetNextWindowCanvasSelector("#canvas");

  // create the only window
  auto window = glfwCreateWindow(600, 400, "Offscreen Canvas | emscripten-glfw", nullptr, nullptr);
  if(!window)
    return -1;

  // make context current
  glfwMakeContextCurrent(window);

  // makes the canvas resizable to the size of its div container
  emscripten::glfw3::MakeCanvasResizable(window, "#canvas-container");

  // set callback for exit (CTRL+Q) and fullscreen (CTRL+F)
  glfwSetKeyCallback(window, onKeyChange);

  // tell emscripten to use "main_loop" as the main loop (window is user data)
  emscripten_set_main_loop_arg(main_loop, window, 0, GLFW_FALSE);
}
