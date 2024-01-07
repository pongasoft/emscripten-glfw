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

//! EmscriptenEventCallback
template<typename E>
using EmscriptenEventCallback = EM_BOOL (*)(int, E const *, void *);

//! EmscriptenListenerFunction
template<typename E>
using EmscriptenListenerFunction = EMSCRIPTEN_RESULT (*)(const char *, void *, EM_BOOL, EmscriptenEventCallback<E>, pthread_t);

//! EmscriptenListenerFunction2 (implicit target)
template<typename E>
using EmscriptenListenerFunction2 = EMSCRIPTEN_RESULT (*)(void *, EM_BOOL, EmscriptenEventCallback<E>, pthread_t);


template<typename E>
class EventListener
{
public:
  using event_listener_t = std::function<bool(int iEventType, E const *iEvent)>;

public:

  EventListener &target(char const *iTarget);
  EventListener &listener(event_listener_t v) { fEventListener = std::move(v); return *this; }
  EventListener &useCapture(bool v) { fUseCapture = v; return *this; }

  bool add(EmscriptenListenerFunction<E> iListenerFunction);
  bool add(EmscriptenListenerFunction2<E> iListenerFunction);

  void remove() { if(fRemoveListenerFunction) fRemoveListenerFunction(); }
  bool invoke(int iEventType, E const *iEvent) { if(fEventListener) return fEventListener(iEventType, iEvent); else return false; }

  ~EventListener() { remove(); }

private:
  using remove_listener_function_t = std::function<void(void)>;

private:
  char const *fSpecialTarget{};
  std::string fTarget{};
  event_listener_t fEventListener{};
  bool fUseCapture{false};
  pthread_t fThread{EM_CALLBACK_THREAD_CONTEXT_CALLING_THREAD};
  remove_listener_function_t fRemoveListenerFunction{};
};

//------------------------------------------------------------------------
// EventListenerCallback
// - generic callback which extracts EventListener<E> from iUserData and invoke it
//------------------------------------------------------------------------
template<typename E>
EventListener<E> &EventListener<E>::target(char const *iTarget)
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

  return *this;
}

//------------------------------------------------------------------------
// EventListenerCallback
// - generic callback which extracts EventListener<E> from iUserData and invoke it
//------------------------------------------------------------------------
template<typename E>
EM_BOOL EventListenerCallback(int iEventType, E const *iEvent, void *iUserData)
{
  auto cb = reinterpret_cast<EventListener<E> *>(iUserData);
  return (*cb).invoke(iEventType, iEvent) ? EM_TRUE : EM_FALSE;
}

//------------------------------------------------------------------------
// EventListener<E>::add
//------------------------------------------------------------------------
template<typename E>
bool EventListener<E>::add(EmscriptenListenerFunction<E> iListenerFunction)
{
  remove();

  auto error = iListenerFunction(fSpecialTarget ? fSpecialTarget : fTarget.c_str(),
                                 this,
                                 fUseCapture ? EM_TRUE : EM_FALSE,
                                 EventListenerCallback<E>,
                                 fThread);

  if(error != EMSCRIPTEN_RESULT_SUCCESS)
  {
    ErrorHandler::instance().logError(GLFW_PLATFORM_ERROR, "Error [%d] while registering listener for [%s]",
                                      error,
                                      fTarget.c_str());
    fRemoveListenerFunction = {};
    return false;
  }

  fRemoveListenerFunction = [iListenerFunction, this]() {
    auto error = iListenerFunction(fSpecialTarget ? fSpecialTarget : fTarget.c_str(),
                                   nullptr,
                                   fUseCapture ? EM_TRUE : EM_FALSE,
                                   nullptr,
                                   fThread);

    if(error != EMSCRIPTEN_RESULT_SUCCESS)
    {
      ErrorHandler::instance().logError(GLFW_PLATFORM_ERROR, "Error [%d] while removing listener for [%s]",
                                        error,
                                        fTarget.c_str());
    }
  };

  return true;
}

//------------------------------------------------------------------------
// EventListener<E>::add
//------------------------------------------------------------------------
template<typename E>
bool EventListener<E>::add(EmscriptenListenerFunction2<E> iListenerFunction)
{
  remove();

  auto error = iListenerFunction(this,
                                 fUseCapture ? EM_TRUE : EM_FALSE,
                                 EventListenerCallback<E>,
                                 fThread);

  if(error != EMSCRIPTEN_RESULT_SUCCESS)
  {
    ErrorHandler::instance().logError(GLFW_PLATFORM_ERROR, "Error [%d] while registering listener", error);
    fRemoveListenerFunction = {};
    return false;
  }

  fRemoveListenerFunction = [iListenerFunction, useCapture = fUseCapture, thread = fThread]() {
    auto error = iListenerFunction(nullptr,
                                   useCapture ? EM_TRUE : EM_FALSE,
                                   nullptr,
                                   thread);

    if(error != EMSCRIPTEN_RESULT_SUCCESS)
    {
      ErrorHandler::instance().logError(GLFW_PLATFORM_ERROR, "Error [%d] while removing listener", error);
    }
  };

  return true;

}


}
#endif //EMSCRIPTEN_GLFW_EVENTS_H
