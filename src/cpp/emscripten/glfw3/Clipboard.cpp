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

#include "Clipboard.h"

extern "C" {
double emscripten_glfw3_context_get_now();
void emscripten_glfw3_context_set_clipboard_string(char const *iContent);
void emscripten_glfw3_context_async_get_clipboard_string();
}

namespace emscripten::glfw3 {

namespace clipboard {

//------------------------------------------------------------------------
// clipboard::Timing::update
//------------------------------------------------------------------------
void Timing::update()
{
  fTime = emscripten_glfw3_context_get_now();
}

}

//------------------------------------------------------------------------
// OSClipboard::update
//------------------------------------------------------------------------
void OSClipboard::update(char const *iText, char const *iError)
{
  fLastModified.update();
  if(iText)
  {
    fText = iText;
    fError = std::nullopt;
  }
  if(iError)
  {
    fText = std::nullopt;
    fError = iError;
  }

  for(auto &promise: fTextPromises)
    promise.set_value(iText ? ClipboardString::fromValue(iText) : ClipboardString::fromError(iError));
  fTextPromises.clear();

  for(auto &callback: fTextCallbacks)
    callback.fCallback(callback.fUserData, iText, iError);
  fTextCallbacks.clear();
}

//------------------------------------------------------------------------
// OSClipboard::writeText
//------------------------------------------------------------------------
void OSClipboard::writeText(char const *iText)
{
  printf("OSClipboard::writeText: [%s]\n", iText ? iText : "");
  if(iText)
  {
    update(iText, nullptr);
    emscripten_glfw3_context_set_clipboard_string(iText);
  }
}

//------------------------------------------------------------------------
// OSClipboard::writeText
//------------------------------------------------------------------------
void OSClipboard::onTextWritten(char const *iText, char const *iError)
{
  update(iText, iError);
  printf("OSClipboard::onTextWritten: [%s] [%s] [%.0f]\n", iText ? iText : "", iError ? iError : "", fLastModified.fTime);
}

//------------------------------------------------------------------------
// OSClipboard::readText
//------------------------------------------------------------------------
bool OSClipboard::readText(double iLastKnownFocusedTime)
{
  if(fTextPromises.empty() && fTextCallbacks.empty())
  {
    if(iLastKnownFocusedTime > 0 // we know the time when the currently focused window was focused
       && !isUnknown() // we have a valid clipboard
       && iLastKnownFocusedTime < fLastModified.fTime ) // it was BEFORE the clipboard was modified
    {
      // we have determined that the clipboard that we received last cannot have changed because the window
      // never lost focus, so there is no need to fetch it again
      printf("OSClipboard::readText: -> detected no need to read\n");
      return false;
    }
    fReadRequest.update();
    printf("OSClipboard::readText(%.0f): [%.0f]\n", iLastKnownFocusedTime, fReadRequest.fTime);
    emscripten_glfw3_context_async_get_clipboard_string();
  }
  return true;
}

//------------------------------------------------------------------------
// OSClipboard::onTextRead
//------------------------------------------------------------------------
double OSClipboard::onTextRead(char const *iText, char const *iError)
{
  update(iText, iError);
  printf("OSClipboard::onTextRead: [%s] [%s] [%.0f] -> %.0f\n", iText ? iText : "", iError ? iError : "", fLastModified.fTime, fLastModified.fTime - fReadRequest.fTime);
  return fLastModified.fTime - fReadRequest.fTime;
}

//------------------------------------------------------------------------
// OSClipboard::setText
//------------------------------------------------------------------------
void OSClipboard::setText(char const *iText)
{
  update(iText, nullptr);
  printf("OSClipboard::setText: [%s] [%.0f]\n", iText ? iText : "", fLastModified.fTime);
}

//------------------------------------------------------------------------
// Clipboard::setText
//------------------------------------------------------------------------
void Clipboard::setText(char const *iText)
{
  if(!iText)
    return;

  fLastModified.update();
  fText = iText;
  fOSClipboard.writeText(iText);
}

//------------------------------------------------------------------------
// Clipboard::getText
//------------------------------------------------------------------------
clipboard::text_t const &Clipboard::getText() const
{
  if(!fOSClipboard.isUnknown() && fOSClipboard.getLastModified().fTime > fLastModified.fTime)
    return fOSClipboard.getText();
  else
    return fText;
}

//------------------------------------------------------------------------
// Clipboard::asyncGetClipboardString
//------------------------------------------------------------------------
std::future<ClipboardString> Clipboard::asyncGetClipboardString(double iLastKnownFocusedTime)
{
  if(fOSClipboard.readText(iLastKnownFocusedTime))
    return fOSClipboard.fTextPromises.emplace_back().get_future();
  else
  {
    auto promise = std::promise<ClipboardString>{};
    auto text = getText();
    promise.set_value(ClipboardString::fromValue(text ? *text : ""));
    return promise.get_future();
  }
}

//------------------------------------------------------------------------
// Clipboard::getClipboardString
//------------------------------------------------------------------------
void Clipboard::getClipboardString(double iLastKnownFocusedTime,
                                   emscripten_glfw_clipboard_string_fun iCallback,
                                   void *iUserData)
{
  if(!iCallback)
    return;

  if(fOSClipboard.readText(iLastKnownFocusedTime))
    fOSClipboard.fTextCallbacks.emplace_back(OSClipboard::ClipboardStringCallback{iCallback, iUserData});
  else
  {
    auto promise = std::promise<ClipboardString>{};
    auto text = getText();
    iCallback(iUserData, text ? text->c_str() : "", nullptr);
  }
}


}