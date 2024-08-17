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
public:
  constexpr bool isUnknown() const { return !fText.has_value(); }
  constexpr clipboard::Timing const &getLastModified() const { return fLastModified; }
  clipboard::text_t const &getText() const { return fText; }

  // navigator.clipboard.writeText
  void writeText(char const *iText);

  friend class Clipboard;

private:
  void update(char const *iText, char const *iError);

private:
  std::optional<std::string> fText{};
  std::optional<std::string> fError{};
  clipboard::Timing fLastModified{};
};

class Clipboard
{
public:
  void onClipboard(char const *iText, char const *iError) { fOSClipboard.update(iText, iError); };

  void setText(char const *iText);
  clipboard::text_t const &getText() const;

private:
  clipboard::text_t fText{};
  clipboard::Timing fLastModified{};

  OSClipboard fOSClipboard{};
};

}

#endif //EMSCRIPTEN_GLFW_CLIPBOARD_H