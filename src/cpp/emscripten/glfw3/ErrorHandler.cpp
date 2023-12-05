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

#include "ErrorHandler.h"
#include <utility>

namespace emscripten::glfw3 {

//------------------------------------------------------------------------
// ErrorHandler::setErrorCallback
//------------------------------------------------------------------------
GLFWerrorfun ErrorHandler::setErrorCallback(GLFWerrorfun iCallback)
{
  std::swap(iCallback, fErrorCallback);
  return iCallback;
}

//------------------------------------------------------------------------
// ErrorHandler::popError
//------------------------------------------------------------------------
int ErrorHandler::popError(char const **iDescription)
{
  int res = fLastErrorCode;
  if(fLastErrorCode != GLFW_NO_ERROR && iDescription)
  {
    fLastErrorCode = GLFW_NO_ERROR;
    *iDescription = fLastErrorMessage.data();
  }
  return res;
}

//------------------------------------------------------------------------
// ErrorHandler::logError
//------------------------------------------------------------------------
void ErrorHandler::logError(int iErrorCode, char const *iErrorMessage)
{
  fLastErrorCode = iErrorCode;
  fLastErrorMessage = iErrorMessage;
  if(fErrorCallback)
    fErrorCallback(fLastErrorCode, fLastErrorMessage.data());
}

}

