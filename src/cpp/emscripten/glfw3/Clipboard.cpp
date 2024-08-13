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

}