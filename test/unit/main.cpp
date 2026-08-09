/*
 * Copyright (c) 2026 pongasoft
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

#include <cstdio>
#include <exception>
#include <sstream>
#include "Mouse.h"

/**
 * This program will run unit tests */

using namespace emscripten::glfw3;

static void throwException(char const *iMessage, char const *iFile, int iLine)
{
  std::ostringstream ss;
  ss << iFile << ":" << iLine << " | " << iMessage;
  throw std::logic_error(ss.str());
}

#define CHECK(test) (test) == true ? (void)0 : throwException("CHECK FAILED: \"" #test "\"", __FILE__, __LINE__)

void test_Mouse_Buttons()
{
  constexpr unsigned short kLeft = GLFW_MOUSE_BUTTON_LEFT;
  constexpr unsigned short kLast = GLFW_MOUSE_BUTTON_LAST;
  constexpr unsigned short kUnlimited1 = GLFW_MOUSE_BUTTON_LAST + 1;

  Mouse::Buttons buttons;

  CHECK(!buttons.isPressed(kLeft));
  CHECK(!buttons.isPressed(kLast));
  CHECK(!buttons.isPressed(kUnlimited1));
  CHECK(!buttons.isSticky(kLeft));
  CHECK(!buttons.isSticky(kLast));
  CHECK(!buttons.isSticky(kUnlimited1));

  buttons.press(kLeft);
  CHECK(buttons.isPressed(kLeft));
  CHECK(!buttons.isPressed(kLast));
  CHECK(!buttons.isPressed(kUnlimited1));
  CHECK(!buttons.isSticky(kLeft));
  CHECK(!buttons.isSticky(kLast));
  CHECK(!buttons.isSticky(kUnlimited1));

  buttons.press(kLast);
  CHECK(buttons.isPressed(kLeft));
  CHECK(buttons.isPressed(kLast));
  CHECK(!buttons.isPressed(kUnlimited1));
  CHECK(!buttons.isSticky(kLeft));
  CHECK(!buttons.isSticky(kLast));
  CHECK(!buttons.isSticky(kUnlimited1));

  buttons.press(kUnlimited1);
  CHECK(buttons.isPressed(kLeft));
  CHECK(buttons.isPressed(kLast));
  CHECK(buttons.isPressed(kUnlimited1));
  CHECK(!buttons.isSticky(kLeft));
  CHECK(!buttons.isSticky(kLast));
  CHECK(!buttons.isSticky(kUnlimited1));

  buttons.release(kLeft, false);
  buttons.release(kLast, true);
  buttons.release(kUnlimited1, true); // should have no effect (cannot be sticky)

  CHECK(!buttons.isPressed(kLeft));
  CHECK(!buttons.isPressed(kLast));
  CHECK(!buttons.isPressed(kUnlimited1));
  CHECK(!buttons.isSticky(kLeft));
  CHECK(buttons.isSticky(kLast));
  CHECK(!buttons.isSticky(kUnlimited1));

  buttons.releaseSticky(kLast);
  CHECK(!buttons.isSticky(kLast));
}

int main()
{
  try
  {
    printf("Running tests...\n");
    int i = 0;
    printf("[%d] test_Mouse_Buttons\n", i++);
    test_Mouse_Buttons();
    printf("Tests completed successfully...\n");
    exit(0);
  }
  catch(std::logic_error const &e)
  {
    printf("Tests failed...\n");
    printf("%s\n", e.what());
    exit(1);
  }
}
