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

/**
 * The purpose of this example is to demonstrate how to make the canvas resizable and occupy the full window. */

//! Display error message in the Console
void consoleErrorHandler(int iErrorCode, char const *iErrorMessage)
{
  printf("glfwError: %d | %s\n", iErrorCode, iErrorMessage);
}

//! Called for each frame
bool renderFrame(GLFWwindow *iWindow)
{
  static int frameCount = 0;

  // poll events
  glfwPollEvents();

  // for the sake of this example, uses the canvas2D api to change the color of the screen / display a message
  int w,h;
  glfwGetFramebufferSize(iWindow, &w, &h);
  int grey = frameCount++ % 255;
  EM_ASM(Module.renderFrame($0, $1, $2, $3, $4, $5), iWindow, w, h, grey, grey, grey);

  // shall we continue?
  return !glfwWindowShouldClose(iWindow);
}

//! The main loop (called by emscripten for each frame)
void main_loop(void *iUserData)
{
  auto window = reinterpret_cast<GLFWwindow *>(iUserData);
  if(!renderFrame(window))
  {
    // done => terminating
    glfwTerminate();
    emscripten_cancel_main_loop();
  }
}

//! Handle key events => on CTRL+Q sets "window should close" flag
void onKeyChange(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if(action == GLFW_PRESS && key == GLFW_KEY_Q && (mods & GLFW_MOD_CONTROL))
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//! main
int main()
{
  // set a callback for errors otherwise it is silent
  glfwSetErrorCallback(consoleErrorHandler);

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

  // set callback for exit (CTRL+Q)
  glfwSetKeyCallback(window, onKeyChange);

  // tell emscripten to use "main_loop" as the main loop (window is user data)
  emscripten_set_main_loop_arg(main_loop, window, 0, GLFW_FALSE);
}
