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

#ifndef EMSCRIPTEN_GLFW_ERRORHANDLER_H
#define EMSCRIPTEN_GLFW_ERRORHANDLER_H

#include <GLFW/glfw3.h>
#include <string>
#include <cstdio>
#include <set>

namespace emscripten::glfw3 {

class ErrorHandler
{
public:
  GLFWerrorfun setErrorCallback(GLFWerrorfun iCallback);
  int popError(const char** iDescription);

  template<typename ... Args>
  void logError(int iErrorCode, char const *iErrorMessage, Args... args);

  template<typename ... Args>
  void logWarning(char const *iWarningMessage, Args... args);

public:
  static ErrorHandler &instance();

private:
  void doLogError(int iErrorCode, char const *iErrorMessage);
#ifndef EMSCRIPTEN_GLFW3_DISABLE_WARNING
  void doLogWarning(char const *iWarningMessage);
#endif

private:
  GLFWerrorfun fErrorCallback{};
  int fLastErrorCode{GLFW_NO_ERROR};
  std::string fLastErrorMessage{};
#ifndef EMSCRIPTEN_GLFW3_DISABLE_WARNING
  std::set<std::string> fWarningMessages{};
#endif
};

//------------------------------------------------------------------------
// ErrorHandler::logError
//------------------------------------------------------------------------
template<typename... Args>
void ErrorHandler::logError(int iErrorCode, char const *iErrorMessage, Args... args)
{
  if constexpr(sizeof...(args) > 0)
  {
    constexpr int kMessageSize = 1024;
    char message[kMessageSize];
    std::snprintf(message, sizeof(message), iErrorMessage, args ...);
    message[sizeof(message) - 1] = '\0';
    doLogError(iErrorCode, message);
  }
  else
    doLogError(iErrorCode, iErrorMessage);
}

//------------------------------------------------------------------------
// ErrorHandler::logWarning
//------------------------------------------------------------------------
template<typename... Args>
void ErrorHandler::logWarning(char const *iWarningMessage, Args... args)
{
#ifndef EMSCRIPTEN_GLFW3_DISABLE_WARNING
  if constexpr(sizeof...(args) > 0)
  {
    constexpr int kMessageSize = 1024;
    char message[kMessageSize];
    std::snprintf(message, sizeof(message), iWarningMessage, args ...);
    message[sizeof(message) - 1] = '\0';
    doLogWarning(message);
  }
  else
    doLogWarning(iWarningMessage);
#endif
}


}

#endif //EMSCRIPTEN_GLFW_ERRORHANDLER_H
