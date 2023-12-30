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

#include "Triangle.h"
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <emscripten/html5.h>
#include <iostream>
#include <array>

// The code has been inspired by https://github.com/sessamekesh/webgl-tutorials-2023 (MIT License)

constexpr const char* vertexShaderText = R"VST(#version 300 es
precision mediump float;

in vec2 vertexPosition;

void main () {
  gl_Position = vec4(vertexPosition, 0.0, 1.0);
})VST";

constexpr const char* fragmentShaderText = R"FST(#version 300 es
precision mediump float;

out vec4 triangleColor;

void main() {
  triangleColor = vec4(0.294, 0.0, 0.51, 1.0);
})FST";


//------------------------------------------------------------------------
// Triangle::init
//------------------------------------------------------------------------
std::unique_ptr<Triangle> Triangle::init(GLFWwindow *iWindow, char const *iName)
{
  glfwMakeContextCurrent(iWindow);

  // Setup Step 2: Define the shape of the triangle
  float triangleVertices[] = {// Top middle
    0.0f, 0.5f,
    // Bottom left
    -0.5f, -0.5f,
    // Bottom right
    0.5f, -0.5f};
  GLuint triangleGeoBuffer = 0, triangleGeoVAO = 0;
  glGenVertexArrays(1, &triangleGeoVAO);
  glGenBuffers(1, &triangleGeoBuffer);
  glBindVertexArray(triangleGeoVAO);
  glBindBuffer(GL_ARRAY_BUFFER, triangleGeoBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices,
               GL_STATIC_DRAW);
  glBindVertexArray(0);

  // Setup Step 3: Compile vertex and fragment shaders for use with rendering
  GLint success = -1;
  char infoLog[512];

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderText, nullptr);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    return nullptr;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderText, nullptr);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    std::cerr << "Fragment shader compilation failed:\n"
              << infoLog << std::endl;
    return nullptr;
  }

  GLuint helloTriangleProgram = glCreateProgram();
  glAttachShader(helloTriangleProgram, vertexShader);
  glAttachShader(helloTriangleProgram, fragmentShader);
  glLinkProgram(helloTriangleProgram);
  glGetProgramiv(helloTriangleProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(helloTriangleProgram, 512, nullptr, infoLog);
    std::cerr << "Failed to LINK helloTriangleProgram:\n"
              << infoLog << std::endl;
    return nullptr;
  }
  // Resource cleanup is necessary in C++ but not JavaScript - garbage collector
  //  handles WebGL resource cleanup for us in WebGL.
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Setup Step 4: Get vertexPosition vertex shader attribute location
  GLint vertexPositionAttribLocation =
    glGetAttribLocation(helloTriangleProgram, "vertexPosition");
  if (vertexPositionAttribLocation < 0) {
    std::cerr << "Failed to get attrib location for vertexPosition"
              << std::endl;
    return nullptr;
  }

  {
    auto err = glGetError();
    if (err > 0) {
      std::cerr << "OpenGL error - " << err << std::endl;
      return nullptr;
    }
  }

  return std::unique_ptr<Triangle>(new Triangle(iWindow,
                                                iName,
                                                helloTriangleProgram,
                                                vertexPositionAttribLocation,
                                                triangleGeoVAO));
}

//------------------------------------------------------------------------
// Triangle::render
//------------------------------------------------------------------------
bool Triangle::render()
{
  glfwMakeContextCurrent(fWindow);
  glClearColor(fBgRed, fBgGreen, fBgBlue, fBgAlpha);
  int width = 0, height = 0;
  glfwGetFramebufferSize(fWindow, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render Step 2: Tell WebGL to use our program for any upcoming draw calls.
  // Notify OpenGL about all attribute slots that need to be used.
  glUseProgram(fProgram);
  glEnableVertexAttribArray(fVertexPositionAttribLocation);

  // Render Step 3: Tell OpenGL to pull vertexPosition attrib from
  // triangleGeoBuffer
  glBindVertexArray(fTriangleGeoVAO);

  // Tell OpenGL that vertexPosition comes from the current ARRAY_BUFFER, in
  // sets
  //  of two floats each, starting 8 bytes apart from each other with no
  //  offset from the beginning of the buffer
  glVertexAttribPointer(
    /* index: vertex attrib location (got earlier) */
    fVertexPositionAttribLocation,
    /* size: number of components in this attribute (vec2 = 2) */
    2,
    /* type: type of data in this attribute (vec2 = float) */
    GL_FLOAT,
    /* normalized: only used for int values (true to map int inputs to float
       range 0, 1) */
    GL_FALSE,
    /* stride: how far to move forward in the buffer for the next element,
       in bytes (2 floats) */
    sizeof(float) * 2,
    /* offset: how far from the first element in the buffer to start looking
       for data */
    (void*)0);

  // Render Step 4: Execute the draw call to actually dispatch to GPU
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // Check for errors...
  {
    auto err = glGetError();
    if (err > 0) {
      std::cerr << "OpenGL error - " << err << std::endl;
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------
// Triangle::setBgColor
//------------------------------------------------------------------------
void Triangle::setBgColor(GLfloat iRed, GLfloat iGreen, GLfloat iBlue, GLfloat iAlpha)
{
  fBgRed = iRed;
  fBgGreen = iGreen;
  fBgBlue = iBlue;
  fBgAlpha = iAlpha;
}

//------------------------------------------------------------------------
// Triangle::shouldClose
//------------------------------------------------------------------------
bool Triangle::shouldClose() const
{
  return glfwWindowShouldClose(fWindow);
}

//------------------------------------------------------------------------
// Triangle::~Triangle
//------------------------------------------------------------------------
Triangle::~Triangle()
{
  glfwSetWindowUserPointer(fWindow, nullptr);
  glfwDestroyWindow(fWindow);
}

//------------------------------------------------------------------------
// fmt
//------------------------------------------------------------------------
template<std::size_t Size, typename... Args>
std::string_view fmt(std::array<char, Size> &oBuffer, char const *iFormat, Args... args)
{
  static_assert(Size > 0);

  auto size = std::snprintf(oBuffer.data(), oBuffer.size(), iFormat, args ...);
  if(size > 0)
  {
    oBuffer[oBuffer.size() - 1] = '\0';
    return {oBuffer.data(), static_cast<std::size_t>(size)};
  }
  else
    return {};
}

//------------------------------------------------------------------------
// setHtmlValue
//------------------------------------------------------------------------
void setHtmlValue(std::string_view iElementSelector, std::string_view iValue)
{
  EM_ASM({
           const element = document.querySelector(UTF8ToString($0));
           if(element)
           {
             const value = UTF8ToString($1);
             if(element.innerHTML !== value)
               element.innerHTML = value;
           }

         }, iElementSelector.data(), iValue.data());
}

//------------------------------------------------------------------------
// setHtmlValue
//------------------------------------------------------------------------
template<typename... Args>
void setHtmlValue(GLFWwindow *iWindow, char const *iFunctionName, char const *iFormat, Args... args)
{
  static std::array<char, 256> kSelector;
  static std::array<char, 256> kValue;
  auto triangle = reinterpret_cast<Triangle *>(glfwGetWindowUserPointer(iWindow));
  if(triangle)
  {
    auto selector = fmt(kSelector, ".%s .%s", iFunctionName, triangle->getName());
    auto value = fmt(kValue, iFormat, std::forward<Args>(args)...);
    setHtmlValue(selector, value);
  }
}

inline char const* actionToString(int action)
{
  char const *a;
  switch(action)
  {
    case GLFW_PRESS: a = "P"; break;
    case GLFW_RELEASE: a = "R"; break;
    case GLFW_REPEAT: a = "H"; break;
    default: a = "U"; break;
  }
  return a;
}

inline char const *cursorModeToString(int iCursorMode)
{
  char const *a;
  switch(iCursorMode)
  {
    case GLFW_CURSOR_NORMAL: a = "Normal"; break;
    case GLFW_CURSOR_HIDDEN: a = "Hidden"; break;
    case GLFW_CURSOR_DISABLED: a = "Locked"; break;
    default: a = "U"; break;
  }
  return a;
}

void onContentScaleChange(GLFWwindow *window, float xScale, float yScale)
{
  setHtmlValue(window, "glfwSetWindowContentScaleCallback", "%.2fx%.2f", xScale, yScale);
}
void onWindowSizeChange(GLFWwindow* window, int width, int height)
{
  setHtmlValue(window, "glfwSetWindowSizeCallback", "%dx%d", width, height);
}
void onFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
  setHtmlValue(window, "glfwSetFramebufferSizeCallback", "%dx%d", width, height);
}
void onCursorPosChange(GLFWwindow *window, double xScale, double yScale)
{
  setHtmlValue(window, "glfwSetCursorPosCallback", "%.2fx%.2f", xScale, yScale);
}
void onMouseButtonChange(GLFWwindow* window, int button, int action, int mods)
{
//  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
//  {
//    int wx, wy, fbx, fby;
//    glfwGetWindowSize(window, &wx, &wy);
//    glfwGetFramebufferSize(window, &fbx, &fby);
//    printf("%dx%d | %dx%d\n", wx, wy, fbx, fby);
//  }
  setHtmlValue(window, "glfwSetMouseButtonCallback", "%d:%s:%d", button, actionToString(action), mods);
}
void onCursorEnterChange(GLFWwindow* window, int entered)
{
  setHtmlValue(window, "glfwSetCursorEnterCallback", "%s", entered == GLFW_TRUE ? "true" : "false");
}
void onScrollChange(GLFWwindow* window, double xoffset, double yoffset)
{
  setHtmlValue(window, "glfwSetScrollCallback", "%.3fx%.3f", xoffset, yoffset);
}
void onKeyChange(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  static constexpr auto adjust = [](int v, float f) { return static_cast<int>(static_cast<float>(v) * f); };

  setHtmlValue(window, "glfwSetKeyCallback", "%d:%d:%s:%d", key, scancode, actionToString(action), mods);
  if(action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL))
  {
    switch(key)
    {
      case GLFW_KEY_H:
      {
        auto mode = glfwGetInputMode(window, GLFW_CURSOR);
        if(mode == GLFW_CURSOR_NORMAL)
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        if(mode == GLFW_CURSOR_HIDDEN)
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
      }
      case GLFW_KEY_L:
      {
        auto mode = glfwGetInputMode(window, GLFW_CURSOR);
        if(mode == GLFW_CURSOR_DISABLED)
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
      }
      case GLFW_KEY_MINUS: // zoom out (-10%)
      {
        int w,h;
        glfwGetWindowSize(window, &w, &h);
        glfwSetWindowSize(window, adjust(w, 0.9), adjust(h, 0.9));
        break;
      }
      case GLFW_KEY_EQUAL: // zoom in (+10%)
      {
        int w,h;
        glfwGetWindowSize(window, &w, &h);
        glfwSetWindowSize(window, adjust(w, 1.1), adjust(h, 1.1));
        break;
      }
      case GLFW_KEY_Q: // close window
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;

      default:
        // ignore
        break;
    }
  }
}
void onCharChange(GLFWwindow* window, unsigned int codepoint)
{
  setHtmlValue(window, "glfwSetCharCallback", "%u", codepoint);
}
void onWindowFocusChange(GLFWwindow* window, int focused)
{
  setHtmlValue(window, "glfwSetWindowFocusCallback", "%s", focused == GLFW_TRUE ? "true" : "false");
}


//------------------------------------------------------------------------
// registerCallbacks
//------------------------------------------------------------------------
void Triangle::registerCallbacks()
{
  glfwSetWindowUserPointer(fWindow, this);
  glfwSetWindowContentScaleCallback(fWindow, onContentScaleChange);
  glfwSetWindowSizeCallback(fWindow, onWindowSizeChange);
  glfwSetFramebufferSizeCallback(fWindow, onFramebufferSizeChange);
  glfwSetCursorPosCallback(fWindow, onCursorPosChange);
  glfwSetMouseButtonCallback(fWindow, onMouseButtonChange);
  glfwSetCursorEnterCallback(fWindow, onCursorEnterChange);
  glfwSetScrollCallback(fWindow, onScrollChange);
  glfwSetKeyCallback(fWindow, onKeyChange);
  glfwSetCharCallback(fWindow, onCharChange);
  glfwSetWindowFocusCallback(fWindow, onWindowFocusChange);
}

//------------------------------------------------------------------------
// updateValues
//------------------------------------------------------------------------
void Triangle::updateValues()
{
  double xd, yd;
  float xf, yf;
  int xi, yi;


  glfwGetWindowSize(fWindow, &xi, &yi);
  setHtmlValue(fWindow, "glfwGetWindowSize", "%dx%d", xi, yi);

  glfwGetFramebufferSize(fWindow, &xi, &yi);
  setHtmlValue(fWindow, "glfwGetFramebufferSize", "%dx%d", xi, yi);

  glfwGetCursorPos(fWindow, &xd, &yd);
  setHtmlValue(fWindow, "glfwGetCursorPos", "%.2fx%.2f", xd, yd);

  setHtmlValue(fWindow, "glfwGetInputMode_GLFW_CURSOR", "%s", cursorModeToString(glfwGetInputMode(fWindow, GLFW_CURSOR)));

  setHtmlValue(fWindow, "glfwGetMouseButton", "L:%s|M:%s|R:%s",
               actionToString(glfwGetMouseButton(fWindow, GLFW_MOUSE_BUTTON_LEFT)),
               actionToString(glfwGetMouseButton(fWindow, GLFW_MOUSE_BUTTON_MIDDLE)),
               actionToString(glfwGetMouseButton(fWindow, GLFW_MOUSE_BUTTON_RIGHT)));

  setHtmlValue(fWindow, "glfwGetKey", "A:%s|Q:%s|Z:%s",
               actionToString(glfwGetKey(fWindow, GLFW_KEY_A)),
               actionToString(glfwGetKey(fWindow, GLFW_KEY_Q)),
               actionToString(glfwGetKey(fWindow, GLFW_KEY_Z)));

  glfwGetWindowContentScale(fWindow, &xf, &yf);
  setHtmlValue(fWindow, "glfwGetWindowContentScale", "%.2fx%.2f", xf, yf);
}

