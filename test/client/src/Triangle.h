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

#ifndef EMSCRIPTEN_GLFW_HELLO_TRIANGLE_H
#define EMSCRIPTEN_GLFW_HELLO_TRIANGLE_H

#include <memory>
#include <GLES3/gl3.h>

struct GLFWwindow;

class Triangle
{
public:
  static std::unique_ptr<Triangle> init(GLFWwindow *iWindow, char const *iName);
  ~Triangle();

  bool render();
  void updateValues();

  void setBgColor(GLfloat iRed, GLfloat iGreen, GLfloat iBlue, GLfloat iAlpha = 1.0f);
  bool shouldClose() const;
  constexpr char const *getName() const { return fName; };
  void onKeyChange(int iKey, int scancode, int iAction, int iMods);

  void registerCallbacks();

private:
  Triangle(GLFWwindow *iWindow, char const *iName, GLuint iProgram, GLint iVertexPositionAttribLocation, GLuint iTriangleGeoVAO) :
    fWindow{iWindow},
    fName{iName},
    fProgram{iProgram},
    fVertexPositionAttribLocation{iVertexPositionAttribLocation},
    fTriangleGeoVAO{iTriangleGeoVAO}
  {}

private:
  GLFWwindow *fWindow;
  char const *fName;
  GLuint fProgram;
  GLint fVertexPositionAttribLocation;
  GLuint fTriangleGeoVAO;
  GLfloat fBgRed{0};
  GLfloat fBgGreen{0};
  GLfloat fBgBlue{0};
  GLfloat fBgAlpha{1.0f};
  int fCursor{};
};


#endif //EMSCRIPTEN_GLFW_HELLO_TRIANGLE_H