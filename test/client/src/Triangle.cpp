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
#include <iostream>

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
std::unique_ptr<Triangle> Triangle::init(GLFWwindow *iWindow)
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
