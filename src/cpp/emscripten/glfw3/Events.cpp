/*
 * Copyright (c) 2025 pongasoft
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

#include "Events.h"

namespace emscripten::glfw3 {

//------------------------------------------------------------------------
// EventListenerBase::setTarget
//------------------------------------------------------------------------
void EventListenerBase::setTarget(char const *iTarget)
{
  if(iTarget == EMSCRIPTEN_EVENT_TARGET_WINDOW)
  {
    fSpecialTarget = EMSCRIPTEN_EVENT_TARGET_WINDOW;
    fTarget = "window";
  } else if(iTarget == EMSCRIPTEN_EVENT_TARGET_DOCUMENT)
  {
    fSpecialTarget = EMSCRIPTEN_EVENT_TARGET_DOCUMENT;
    fTarget = "document";
  } else if(iTarget == EMSCRIPTEN_EVENT_TARGET_SCREEN)
  {
    fSpecialTarget = EMSCRIPTEN_EVENT_TARGET_SCREEN;
    fTarget = "screen";
  } else {
    fTarget = iTarget;
  }
}

//------------------------------------------------------------------------
// EventListenerBase::setCallbackFunction
//------------------------------------------------------------------------
bool EventListenerBase::setCallbackFunction(callback_function_t iFunction)
{
  remove();

  fCallbackFunction = std::move(iFunction);

  auto error = fCallbackFunction(true);

  if(error != EMSCRIPTEN_RESULT_SUCCESS)
  {
    ErrorHandler::instance().logError(GLFW_PLATFORM_ERROR, "Error [%d] while registering listener for [%s]",
                                      error,
                                      fTarget.c_str());
    fCallbackFunction = {};
    return false;
  }

  return true;
}

//------------------------------------------------------------------------
// EventListenerBase::remove
//------------------------------------------------------------------------
void EventListenerBase::remove()
{
  if(fCallbackFunction)
  {
    auto error = fCallbackFunction(false);
    if(error != EMSCRIPTEN_RESULT_SUCCESS)
    {
      ErrorHandler::instance().logError(GLFW_PLATFORM_ERROR, "Error [%d] while removing listener for [%s]",
                                        error,
                                        fTarget.c_str());
    }
  }
}

}
