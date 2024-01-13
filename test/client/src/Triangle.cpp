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
#include <vector>

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
           if(element) {
             const value = UTF8ToString($1);
             if(element instanceof HTMLInputElement) {
               if(element.value !== value)
                 element.value = value;
             } else {
               if(element.innerHTML !== value)
                 element.innerHTML = value;
             }
           }

         }, iElementSelector.data(), iValue.data());
}

//------------------------------------------------------------------------
// hideHTMLElement
//------------------------------------------------------------------------
void hideHTMLElement(std::string_view iElementSelector)
{
  EM_ASM({ Module.hideHTMLElement(UTF8ToString($0)); }, iElementSelector.data());
}

//------------------------------------------------------------------------
// showHTMLElement
//------------------------------------------------------------------------
void showHTMLElement(std::string_view iElementSelector)
{
  EM_ASM({ Module.showHTMLElement(UTF8ToString($0)); }, iElementSelector.data());
}

//------------------------------------------------------------------------
// setHtmlValue
//------------------------------------------------------------------------
template<typename... Args>
void setHtmlValue(GLFWwindow *iWindow, char const *iFunctionName, char const *iFormat, Args... args)
{
  static std::array<char, 256> kSelector;
  static std::array<char, 256> kValue;
  if(iWindow)
  {
    if(auto triangle = reinterpret_cast<Triangle *>(glfwGetWindowUserPointer(iWindow)); triangle)
    {
      auto selector = fmt(kSelector, ".%s .%s", iFunctionName, triangle->getName());
      auto value = fmt(kValue, iFormat, std::forward<Args>(args)...);
      setHtmlValue(selector, value);
    }
  }
  else
  {
    auto selector = fmt(kSelector, ".%s .no-canvas", iFunctionName);
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

inline static char const *toNonNullString(char const *s)
{
  static char const *kNoValue = "-";
  return s ? s : kNoValue;
}

constexpr inline char const *glfwBoolToString(int b) { return b == GLFW_FALSE ?  "false" : "true"; }

void onContentScaleChange(GLFWwindow *window, float xScale, float yScale)
{
  setHtmlValue(window, "glfwSetWindowContentScaleCallback", "%.2fx%.2f", xScale, yScale);
}
void onWindowSizeChange(GLFWwindow* window, int width, int height)
{
  setHtmlValue(window, "glfwSetWindowSizeCallback", "%dx%d", width, height);
  EM_ASM({ Module.onWindowSizeChanged($0, $1, $2); }, window, width, height);
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
  setHtmlValue(window, "glfwSetCursorEnterCallback", "%s", glfwBoolToString(entered));
}
void onScrollChange(GLFWwindow* window, double xoffset, double yoffset)
{
  setHtmlValue(window, "glfwSetScrollCallback", "%.3fx%.3f", xoffset, yoffset);
}
void onKeyChange(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  setHtmlValue(window, "glfwSetKeyCallback", "%d:%d:%s:%d", key, scancode, actionToString(action), mods);
  reinterpret_cast<Triangle *>(glfwGetWindowUserPointer(window))->onKeyChange(key, scancode, action, mods);

}
void onCharChange(GLFWwindow* window, unsigned int codepoint)
{
  setHtmlValue(window, "glfwSetCharCallback", "%u", codepoint);
}
void onWindowFocusChange(GLFWwindow* window, int focused)
{
  setHtmlValue(window, "glfwSetWindowFocusCallback", "%s", glfwBoolToString(focused));
}
void onJoystickChange(int jid, int event)
{
  setHtmlValue(nullptr, "glfwSetJoystickCallback", "%d: %s", jid, event == GLFW_CONNECTED ? "connected" : "disconnected");
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
  glfwSetKeyCallback(fWindow, ::onKeyChange);
  glfwSetCharCallback(fWindow, onCharChange);
  glfwSetWindowFocusCallback(fWindow, onWindowFocusChange);
}

//------------------------------------------------------------------------
// registerNoWindowCallbacks
//------------------------------------------------------------------------
void Triangle::registerNoWindowCallbacks()
{
  glfwSetJoystickCallback(onJoystickChange);
}

//------------------------------------------------------------------------
// updateNoWindowValues
//------------------------------------------------------------------------
void Triangle::updateNoWindowValues()
{
  setHtmlValue(nullptr, "glfwGetTime", "%.2f", glfwGetTime());
  setHtmlValue(nullptr, "glfwGetTimerValue", "%ld", glfwGetTimerValue());

  static std::vector<int> jids{};

  jids.clear();

  for(auto jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++)
  {
    if(glfwJoystickPresent(jid) == GLFW_TRUE)
    {
      jids.emplace_back(jid);
    }
  }

  setHtmlValue(nullptr, "glfwJoystickPresent", "%zu present", jids.size());

  if(!jids.empty())
  {
    showHTMLElement("#joystick");
    auto jid = jids[0];
    setHtmlValue(nullptr, "glfwGetJoystickName", "[%d] %s", jid, toNonNullString(glfwGetJoystickName(jid)));
    setHtmlValue(nullptr, "glfwGetJoystickGUID", "[%d] %s", jid, toNonNullString(glfwGetJoystickGUID(jid)));
    setHtmlValue(nullptr, "glfwJoystickIsGamepad", "[%d] %s", jid, glfwBoolToString(glfwJoystickIsGamepad(jid)));

    int count;
    auto axes = glfwGetJoystickAxes(jid, &count);
    if(axes && count > 0)
    {
      std::string s{};
      for(int i = 0; i < count; i++)
      {
        if(i > 0)
          s += "|";
        s += std::to_string(i) + ":" + std::to_string(axes[i]);
      }
      setHtmlValue(nullptr, "glfwGetJoystickAxes", "[%d] %s", jid, s.c_str());
    }
    else
      setHtmlValue(nullptr, "glfwGetJoystickAxes", "[%d] -", jid);

    auto buttons = glfwGetJoystickButtons(jid, &count);
    if(buttons && count > 0)
    {
      std::string s{};
      for(int i = 0; i < count; i++)
      {
        if(i > 0)
          s += "|";
        s += std::to_string(i) + ":" + (buttons[i] > 0 ? "1" : "0");
      }
      setHtmlValue(nullptr, "glfwGetJoystickButtons", "[%d] %s", jid, s.c_str());
    }
    else
      setHtmlValue(nullptr, "glfwGetJoystickButtons", "[%d] -", jid);

    auto hats = glfwGetJoystickHats(jid, &count);
    if(hats && count > 0)
    {
      std::string s{};
      for(int i = 0; i < count; i++)
      {
        if(i > 0)
          s += "|";
        if(hats[i] == GLFW_HAT_CENTERED)
          s += "C";
        else
        {
          if(hats[i] & GLFW_HAT_UP)
            s += "U";
          if(hats[i] & GLFW_HAT_DOWN)
            s += "D";
          if(hats[i] & GLFW_HAT_LEFT)
            s += "L";
          if(hats[i] & GLFW_HAT_RIGHT)
            s += "R";
        }
      }
      setHtmlValue(nullptr, "glfwGetJoystickHats", "[%d] %s", jid, s.c_str());
    }
    else
      setHtmlValue(nullptr, "glfwGetJoystickHats", "[%d] -", jid);

    GLFWgamepadstate state;
    if(glfwGetGamepadState(jid, &state) == GLFW_TRUE)
    {
      setHtmlValue(nullptr, "glfwGetGamepadState-axes", "[%d] LX:%.2f,LY:%.2f,RX:%.2f,RY:%.2f,LT:%.2f,RT:%.2f ",
                   jid,
                   state.axes[GLFW_GAMEPAD_AXIS_LEFT_X],
                   state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y],
                   state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X],
                   state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y],
                   state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER],
                   state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]
      );
      setHtmlValue(nullptr, "glfwGetGamepadState-buttons",
                   "[%d] A:%d,B:%d,X:%d,Y:%d,LB:%d,RB:%d,Back:%d,Start:%d,Guide:%d,LT:%d,RT:%d,U:%d,R:%d,D:%d,L:%d",
                   jid,
                   state.buttons[GLFW_GAMEPAD_BUTTON_A],
                   state.buttons[GLFW_GAMEPAD_BUTTON_B],
                   state.buttons[GLFW_GAMEPAD_BUTTON_X],
                   state.buttons[GLFW_GAMEPAD_BUTTON_Y],
                   state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER],
                   state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER],
                   state.buttons[GLFW_GAMEPAD_BUTTON_BACK],
                   state.buttons[GLFW_GAMEPAD_BUTTON_START],
                   state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE],
                   state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB],
                   state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB],
                   state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP],
                   state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT],
                   state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN],
                   state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]
      );
    }
  }
  else
  {
    hideHTMLElement("#joystick");
    setHtmlValue(nullptr, "glfwGetGamepadState-axes", "%s", "-");
    setHtmlValue(nullptr, "glfwGetGamepadState-buttons", "%s", "-");
  }
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

  glfwGetWindowPos(fWindow, &xi, &yi);
  setHtmlValue(fWindow, "glfwGetWindowPos", "%dx%d", xi, yi);

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

  setHtmlValue(fWindow, "glfwGetWindowOpacity", "%.2f", glfwGetWindowOpacity(fWindow));

  bool visible = glfwGetWindowAttrib(fWindow, GLFW_VISIBLE) == GLFW_TRUE;
  setHtmlValue(fWindow, "glfwGetWindowAttrib-visible", "%s", visible ? "visible" : "hidden");
  setHtmlValue(fWindow, "glfwShowWindow", "%s", visible ? "Hide" : "Show");

  setHtmlValue(fWindow, "glfwGetWindowAttrib-focused", "%s", glfwBoolToString(glfwGetWindowAttrib(fWindow, GLFW_FOCUSED)));

  setHtmlValue(fWindow, "glfwGetWindowAttrib-resizable", "%s", glfwBoolToString(glfwGetWindowAttrib(fWindow, GLFW_RESIZABLE)));

  auto hiDPIAware = glfwGetWindowAttrib(fWindow, GLFW_SCALE_TO_MONITOR);
  setHtmlValue(fWindow, "glfwGetWindowAttrib-scale_to_monitor", "%s", glfwBoolToString(hiDPIAware));
  setHtmlValue(fWindow, "glfwSetWindowAttrib-scale_to_monitor", "%s", hiDPIAware ? "Disable" : "Enable");
}

static constexpr auto adjust = [](int v, float f) { return static_cast<int>(static_cast<float>(v) * f); };

//------------------------------------------------------------------------
// Triangle::onKeyChange
//------------------------------------------------------------------------
void Triangle::onKeyChange(int iKey, int iScancode, int iAction, int iMods)
{
  static std::array<int, 6> kCursors =
    {GLFW_ARROW_CURSOR, GLFW_IBEAM_CURSOR, GLFW_CROSSHAIR_CURSOR, GLFW_HAND_CURSOR, GLFW_HRESIZE_CURSOR, GLFW_VRESIZE_CURSOR};

  // Handle CTRL + <iKey>
  if(iAction == GLFW_PRESS && (iMods & GLFW_MOD_CONTROL))
  {
    switch(iKey)
    {
      case GLFW_KEY_C: // toggle between input mode GLFW_CURSOR_HIDDEN / GLFW_CURSOR_NORMAL
      {
        auto mode = glfwGetInputMode(fWindow, GLFW_CURSOR);
        if(mode == GLFW_CURSOR_NORMAL)
          glfwSetInputMode(fWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        if(mode == GLFW_CURSOR_HIDDEN)
          glfwSetInputMode(fWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
      }
      case GLFW_KEY_L: // lock / unlock the pointer
      {
        auto mode = glfwGetInputMode(fWindow, GLFW_CURSOR);
        if(mode == GLFW_CURSOR_DISABLED)
          glfwSetInputMode(fWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
          glfwSetInputMode(fWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
      }

      case GLFW_KEY_RIGHT_BRACKET: // cycle through cursor
        fCursor++;
        if(fCursor >= kCursors.size())
          fCursor = 0;
        glfwSetCursor(fWindow, glfwCreateStandardCursor(kCursors[fCursor]));
        break;

      case GLFW_KEY_LEFT_BRACKET: // cycle through cursor
        fCursor--;
        if(fCursor < 0)
          fCursor = kCursors.size() - 1;
        glfwSetCursor(fWindow, glfwCreateStandardCursor(kCursors[fCursor]));
        break;

      case GLFW_KEY_4: // toggle hi dpi aware
        toggleHiDPIAware();
        break;

      default:
        // ignore
        break;
    }
  }
}

//------------------------------------------------------------------------
// Triangle::toggleShow
//------------------------------------------------------------------------
void Triangle::toggleShow()
{
  if(glfwGetWindowAttrib(fWindow, GLFW_VISIBLE) == GLFW_TRUE)
    glfwHideWindow(fWindow);
  else
    glfwShowWindow(fWindow);
}

//------------------------------------------------------------------------
// Triangle::zoomIn
//------------------------------------------------------------------------
void Triangle::zoomIn()
{
  int w,h;
  glfwGetWindowSize(fWindow, &w, &h);
  glfwSetWindowSize(fWindow, adjust(w, 1.1), adjust(h, 1.1));
}

//------------------------------------------------------------------------
// Triangle::zoomOut
//------------------------------------------------------------------------
void Triangle::zoomOut()
{
  int w,h;
  glfwGetWindowSize(fWindow, &w, &h);
  glfwSetWindowSize(fWindow, adjust(w, 0.9), adjust(h, 0.9));
}

//------------------------------------------------------------------------
// Triangle::zoomReset
//------------------------------------------------------------------------
void Triangle::zoomReset()
{
  glfwSetWindowSize(fWindow, 300, 200);
}

//------------------------------------------------------------------------
// Triangle::setOpacity
//------------------------------------------------------------------------
void Triangle::setOpacity(float iOpacity)
{
  glfwSetWindowOpacity(fWindow, iOpacity);
}

//------------------------------------------------------------------------
// Triangle::close
//------------------------------------------------------------------------
void Triangle::close()
{
  glfwSetWindowShouldClose(fWindow, GLFW_TRUE);
}

//------------------------------------------------------------------------
// toggleWindowAttrib
//------------------------------------------------------------------------
static void toggleWindowAttrib(GLFWwindow *iWindow, int iAttrib)
{
  auto value = glfwGetWindowAttrib(iWindow, iAttrib);
  glfwSetWindowAttrib(iWindow, iAttrib, value == GLFW_FALSE ? GLFW_TRUE : GLFW_FALSE);
}

//------------------------------------------------------------------------
// Triangle::toggleHiDPIAware
//------------------------------------------------------------------------
void Triangle::toggleHiDPIAware()
{
  toggleWindowAttrib(fWindow, GLFW_SCALE_TO_MONITOR);
}

//------------------------------------------------------------------------
// Triangle::toggleResizable
//------------------------------------------------------------------------
void Triangle::toggleResizable()
{
  toggleWindowAttrib(fWindow, GLFW_RESIZABLE);
}

//------------------------------------------------------------------------
// Triangle::toggleSizeLimits
//------------------------------------------------------------------------
void Triangle::toggleSizeLimits()
{
  if(fHasSizeLimits)
    glfwSetWindowSizeLimits(fWindow, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE);
  else
    glfwSetWindowSizeLimits(fWindow, 150, 100, 300, 200);
  fHasSizeLimits = !fHasSizeLimits;
}

//------------------------------------------------------------------------
// Triangle::toggleAspectRatio
//------------------------------------------------------------------------
void Triangle::toggleAspectRatio()
{
  if(fHasAspectRatio)
    glfwSetWindowAspectRatio(fWindow, GLFW_DONT_CARE, GLFW_DONT_CARE);
  else
    glfwSetWindowAspectRatio(fWindow, 3, 2);
  fHasAspectRatio = !fHasAspectRatio;
}

//------------------------------------------------------------------------
// Triangle::updateTitle
//------------------------------------------------------------------------
void Triangle::updateTitle()
{
  static int kId = 1;
  static std::array<char, 256> kTitle;
  fmt(kTitle, "%s | Update %d", getName(), kId++);
  glfwSetWindowTitle(fWindow, kTitle.data());
}
