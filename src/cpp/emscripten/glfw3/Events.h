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

#ifndef EMSCRIPTEN_GLFW_EVENTS_H
#define EMSCRIPTEN_GLFW_EVENTS_H

#include <emscripten/html5.h>
#include <functional>
#include "ErrorHandler.h"

namespace emscripten::glfw3 {

//! EventListener
template<typename E>
using EventListener = std::function<bool(int iEventType, E const *iEvent)>;

//! EmscriptenEventCallback
template<typename E>
using EmscriptenEventCallback = EM_BOOL (*)(int, E const *, void *);

//! EmscriptenListenerFunction
template<typename E>
using EmscriptenListenerFunction = EMSCRIPTEN_RESULT (*)(const char *, void *, EM_BOOL, EmscriptenEventCallback<E>, pthread_t);

//------------------------------------------------------------------------
// EventListenerCallback
// - generic callback which extracts EventListener<E> from iUserData and invoke it
//------------------------------------------------------------------------
template<typename E>
EM_BOOL EventListenerCallback(int iEventType, E const *iEvent, void *iUserData)
{
  auto cb = reinterpret_cast<EventListener<E> *>(iUserData);
  return std::invoke(*cb, iEventType, iEvent) ? EM_TRUE : EM_FALSE;
}

//------------------------------------------------------------------------
// addOrRemoveListener
//------------------------------------------------------------------------
template<typename E>
void addOrRemoveListener(EmscriptenListenerFunction<E> iListenerFunction,
                         bool iAdd,
                         char const *iTarget,
                         EventListener<E> *iEventListener,
                         bool iUseCapture,
                         pthread_t iThread = EM_CALLBACK_THREAD_CONTEXT_CALLING_THREAD)
{
  auto error = iListenerFunction(iTarget,
                                 iAdd ? iEventListener : nullptr,
                                 iUseCapture ? EM_TRUE : EM_FALSE,
                                 iAdd ? EventListenerCallback<EmscriptenMouseEvent> : nullptr,
                                 iThread);

  if(error != EMSCRIPTEN_RESULT_SUCCESS)
  {
    ErrorHandler::instance().logError(GLFW_PLATFORM_ERROR, "Error [%d] while registering listener for [%s]",
                                      error,
                                      iTarget);
  }
}

}
#endif //EMSCRIPTEN_GLFW_EVENTS_H
