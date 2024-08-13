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

#ifndef EMSCRIPTEN_GLFW_CLIPBOARD_H
#define EMSCRIPTEN_GLFW_CLIPBOARD_H

#include <string>
#include <optional>
#include <functional>
#include <GLFW/emscripten_glfw3.h>

namespace emscripten::glfw3 {

namespace clipboard {

struct Timing
{
  void update();
  double fTime{};
};

using text_t = std::optional<std::string>;

}

class OSClipboard
{
private:


public:
  constexpr bool isUnknown() const { return !fText.has_value(); }
  constexpr clipboard::Timing const &getLastModified() const { return fLastModified; }
  clipboard::text_t const &getText() const { return fText; }

  // navigator.clipboard.writeText
  void writeText(char const *iText);
  void onTextWritten(char const *iText, char const *iError);

  // navigator.clipboard.readText
  bool readText(double iLastKnownFocusedTime);
  double onTextRead(char const *iText, char const *iError);

  // "paste"/"cut"/"copy" listener
  void setText(char const *iText);

  friend class Clipboard;

private:
  void update(char const *iText, char const *iError);

private:
  struct ClipboardStringCallback
  {
    emscripten_glfw_clipboard_string_fun fCallback{};
    void *fUserData{};
  };

private:
  std::optional<std::string> fText{};
  std::optional<std::string> fError{};
  clipboard::Timing fLastModified{};
  clipboard::Timing fReadRequest{};
  std::vector<std::promise<ClipboardString>> fTextPromises{};
  std::vector<ClipboardStringCallback> fTextCallbacks{};
};

class Clipboard
{
public:
  void onCutCopyOrPaste(char const *iText) { fOSClipboard.setText(iText); }
  double onTextRead(char const *iText, char const *iError) { return fOSClipboard.onTextRead(iText, iError); };
  void onTextWritten(char const *iText, char const *iError) { fOSClipboard.onTextWritten(iText, iError); };

  void setText(char const *iText);
  clipboard::text_t const &getText() const;

  std::future<ClipboardString> asyncGetClipboardString(double iLastKnownFocusedTime);
  void getClipboardString(double iLastKnownFocusedTime, emscripten_glfw_clipboard_string_fun iCallback, void *iUserData = nullptr);

private:
  clipboard::text_t fText{};
  clipboard::Timing fLastModified{};

  OSClipboard fOSClipboard{};
};

}

#endif //EMSCRIPTEN_GLFW_CLIPBOARD_H