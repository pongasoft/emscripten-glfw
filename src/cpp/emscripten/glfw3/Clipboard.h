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
  void reset();
  void update();

  double fTime{};
  double fFrame{};
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

  void invalidate();

  // navigator.clipboard.writeText
  void writeText(char const *iText);
  void onTextWritten(char const *iText, char const *iError);

  // navigator.clipboard.readText
  bool readText(double iLastKnownFocusedTime);
  double onTextRead(char const *iText, char const *iError);

  // "paste" listener
  void pasteText(char const *iText);

  // "copy" listener
  clipboard::text_t const &fetchSelection(char const *iTextSelection);
  clipboard_selection_fun_t setSelectionCallback(std::function<clipboard::text_t()> iCallback) { return std::exchange(fSelectionCallback, std::move(iCallback)); }

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
  clipboard_selection_fun_t fSelectionCallback{};
  std::optional<std::string> fSelection{};
  std::vector<std::promise<ClipboardString>> fTextPromises{};
  std::vector<ClipboardStringCallback> fTextCallbacks{};
};

class Clipboard
{
public:
  void onPaste(char const *iText) { fOSClipboard.pasteText(iText); }
  clipboard::text_t const &onCopy(char const *iTextSelection) { return fOSClipboard.fetchSelection(iTextSelection); };
  double onTextRead(char const *iText, char const *iError) { return fOSClipboard.onTextRead(iText, iError); };
  void onTextWritten(char const *iText, char const *iError) { fOSClipboard.onTextWritten(iText, iError); };

  void setText(char const *iText);
  clipboard::text_t const &getText() const;

  std::future<ClipboardString> asyncGetClipboardString(double iLastKnownFocusedTime);
  void getClipboardString(double iLastKnownFocusedTime, emscripten_glfw_clipboard_string_fun iCallback, void *iUserData = nullptr);

  clipboard_selection_fun_t setClipboardSelectionCallback(clipboard_selection_fun_t iCallback) { return fOSClipboard.setSelectionCallback(std::move(iCallback)); }

private:
  clipboard::text_t fText{};
  clipboard::Timing fLastModified{};

  OSClipboard fOSClipboard{};
};

}

#endif //EMSCRIPTEN_GLFW_CLIPBOARD_H