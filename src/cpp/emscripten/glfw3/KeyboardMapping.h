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

#ifndef EMSCRIPTEN_GLFW_KEYBOARD_MAPPING_H
#define EMSCRIPTEN_GLFW_KEYBOARD_MAPPING_H

#include <GLFW/glfw3.h>

namespace emscripten::glfw3 {

using glfw_scancode_t = int; // ex: DOM_PK_A
using glfw_key_t = int; // ex: GLFW_KEY_A

namespace keyboard {

constexpr glfw_scancode_t DOM_PK_UNKNOWN              = 0x0000; /* "Unidentified"       */
constexpr glfw_scancode_t DOM_PK_ESCAPE               = 0x0001; /* "Escape"             */
constexpr glfw_scancode_t DOM_PK_0                    = 0x0002; /* "Digit0"             */
constexpr glfw_scancode_t DOM_PK_1                    = 0x0003; /* "Digit1"             */
constexpr glfw_scancode_t DOM_PK_2                    = 0x0004; /* "Digit2"             */
constexpr glfw_scancode_t DOM_PK_3                    = 0x0005; /* "Digit3"             */
constexpr glfw_scancode_t DOM_PK_4                    = 0x0006; /* "Digit4"             */
constexpr glfw_scancode_t DOM_PK_5                    = 0x0007; /* "Digit5"             */
constexpr glfw_scancode_t DOM_PK_6                    = 0x0008; /* "Digit6"             */
constexpr glfw_scancode_t DOM_PK_7                    = 0x0009; /* "Digit7"             */
constexpr glfw_scancode_t DOM_PK_8                    = 0x000A; /* "Digit8"             */
constexpr glfw_scancode_t DOM_PK_9                    = 0x000B; /* "Digit9"             */
constexpr glfw_scancode_t DOM_PK_MINUS                = 0x000C; /* "Minus"              */
constexpr glfw_scancode_t DOM_PK_EQUAL                = 0x000D; /* "Equal"              */
constexpr glfw_scancode_t DOM_PK_BACKSPACE            = 0x000E; /* "Backspace"          */
constexpr glfw_scancode_t DOM_PK_TAB                  = 0x000F; /* "Tab"                */
constexpr glfw_scancode_t DOM_PK_Q                    = 0x0010; /* "KeyQ"               */
constexpr glfw_scancode_t DOM_PK_W                    = 0x0011; /* "KeyW"               */
constexpr glfw_scancode_t DOM_PK_E                    = 0x0012; /* "KeyE"               */
constexpr glfw_scancode_t DOM_PK_R                    = 0x0013; /* "KeyR"               */
constexpr glfw_scancode_t DOM_PK_T                    = 0x0014; /* "KeyT"               */
constexpr glfw_scancode_t DOM_PK_Y                    = 0x0015; /* "KeyY"               */
constexpr glfw_scancode_t DOM_PK_U                    = 0x0016; /* "KeyU"               */
constexpr glfw_scancode_t DOM_PK_I                    = 0x0017; /* "KeyI"               */
constexpr glfw_scancode_t DOM_PK_O                    = 0x0018; /* "KeyO"               */
constexpr glfw_scancode_t DOM_PK_P                    = 0x0019; /* "KeyP"               */
constexpr glfw_scancode_t DOM_PK_BRACKET_LEFT         = 0x001A; /* "BracketLeft"        */
constexpr glfw_scancode_t DOM_PK_BRACKET_RIGHT        = 0x001B; /* "BracketRight"       */
constexpr glfw_scancode_t DOM_PK_ENTER                = 0x001C; /* "Enter"              */
constexpr glfw_scancode_t DOM_PK_CONTROL_LEFT         = 0x001D; /* "ControlLeft"        */
constexpr glfw_scancode_t DOM_PK_A                    = 0x001E; /* "KeyA"               */
constexpr glfw_scancode_t DOM_PK_S                    = 0x001F; /* "KeyS"               */
constexpr glfw_scancode_t DOM_PK_D                    = 0x0020; /* "KeyD"               */
constexpr glfw_scancode_t DOM_PK_F                    = 0x0021; /* "KeyF"               */
constexpr glfw_scancode_t DOM_PK_G                    = 0x0022; /* "KeyG"               */
constexpr glfw_scancode_t DOM_PK_H                    = 0x0023; /* "KeyH"               */
constexpr glfw_scancode_t DOM_PK_J                    = 0x0024; /* "KeyJ"               */
constexpr glfw_scancode_t DOM_PK_K                    = 0x0025; /* "KeyK"               */
constexpr glfw_scancode_t DOM_PK_L                    = 0x0026; /* "KeyL"               */
constexpr glfw_scancode_t DOM_PK_SEMICOLON            = 0x0027; /* "Semicolon"          */
constexpr glfw_scancode_t DOM_PK_QUOTE                = 0x0028; /* "Quote"              */
constexpr glfw_scancode_t DOM_PK_BACKQUOTE            = 0x0029; /* "Backquote"          */
constexpr glfw_scancode_t DOM_PK_SHIFT_LEFT           = 0x002A; /* "ShiftLeft"          */
constexpr glfw_scancode_t DOM_PK_BACKSLASH            = 0x002B; /* "Backslash"          */
constexpr glfw_scancode_t DOM_PK_Z                    = 0x002C; /* "KeyZ"               */
constexpr glfw_scancode_t DOM_PK_X                    = 0x002D; /* "KeyX"               */
constexpr glfw_scancode_t DOM_PK_C                    = 0x002E; /* "KeyC"               */
constexpr glfw_scancode_t DOM_PK_V                    = 0x002F; /* "KeyV"               */
constexpr glfw_scancode_t DOM_PK_B                    = 0x0030; /* "KeyB"               */
constexpr glfw_scancode_t DOM_PK_N                    = 0x0031; /* "KeyN"               */
constexpr glfw_scancode_t DOM_PK_M                    = 0x0032; /* "KeyM"               */
constexpr glfw_scancode_t DOM_PK_COMMA                = 0x0033; /* "Comma"              */
constexpr glfw_scancode_t DOM_PK_PERIOD               = 0x0034; /* "Period"             */
constexpr glfw_scancode_t DOM_PK_SLASH                = 0x0035; /* "Slash"              */
constexpr glfw_scancode_t DOM_PK_SHIFT_RIGHT          = 0x0036; /* "ShiftRight"         */
constexpr glfw_scancode_t DOM_PK_NUMPAD_MULTIPLY      = 0x0037; /* "NumpadMultiply"     */
constexpr glfw_scancode_t DOM_PK_ALT_LEFT             = 0x0038; /* "AltLeft"            */
constexpr glfw_scancode_t DOM_PK_SPACE                = 0x0039; /* "Space"              */
constexpr glfw_scancode_t DOM_PK_CAPS_LOCK            = 0x003A; /* "CapsLock"           */
constexpr glfw_scancode_t DOM_PK_F1                   = 0x003B; /* "F1"                 */
constexpr glfw_scancode_t DOM_PK_F2                   = 0x003C; /* "F2"                 */
constexpr glfw_scancode_t DOM_PK_F3                   = 0x003D; /* "F3"                 */
constexpr glfw_scancode_t DOM_PK_F4                   = 0x003E; /* "F4"                 */
constexpr glfw_scancode_t DOM_PK_F5                   = 0x003F; /* "F5"                 */
constexpr glfw_scancode_t DOM_PK_F6                   = 0x0040; /* "F6"                 */
constexpr glfw_scancode_t DOM_PK_F7                   = 0x0041; /* "F7"                 */
constexpr glfw_scancode_t DOM_PK_F8                   = 0x0042; /* "F8"                 */
constexpr glfw_scancode_t DOM_PK_F9                   = 0x0043; /* "F9"                 */
constexpr glfw_scancode_t DOM_PK_F10                  = 0x0044; /* "F10"                */
constexpr glfw_scancode_t DOM_PK_PAUSE                = 0x0045; /* "Pause"              */
constexpr glfw_scancode_t DOM_PK_SCROLL_LOCK          = 0x0046; /* "ScrollLock"         */
constexpr glfw_scancode_t DOM_PK_NUMPAD_7             = 0x0047; /* "Numpad7"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_8             = 0x0048; /* "Numpad8"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_9             = 0x0049; /* "Numpad9"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_SUBTRACT      = 0x004A; /* "NumpadSubtract"     */
constexpr glfw_scancode_t DOM_PK_NUMPAD_4             = 0x004B; /* "Numpad4"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_5             = 0x004C; /* "Numpad5"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_6             = 0x004D; /* "Numpad6"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_ADD           = 0x004E; /* "NumpadAdd"          */
constexpr glfw_scancode_t DOM_PK_NUMPAD_1             = 0x004F; /* "Numpad1"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_2             = 0x0050; /* "Numpad2"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_3             = 0x0051; /* "Numpad3"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_0             = 0x0052; /* "Numpad0"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_DECIMAL       = 0x0053; /* "NumpadDecimal"      */
constexpr glfw_scancode_t DOM_PK_PRINT_SCREEN         = 0x0054; /* "PrintScreen"        */
constexpr glfw_scancode_t DOM_PK_INTL_BACKSLASH       = 0x0056; /* "IntlBackslash"      */
constexpr glfw_scancode_t DOM_PK_F11                  = 0x0057; /* "F11"                */
constexpr glfw_scancode_t DOM_PK_F12                  = 0x0058; /* "F12"                */
constexpr glfw_scancode_t DOM_PK_NUMPAD_EQUAL         = 0x0059; /* "NumpadEqual"        */
constexpr glfw_scancode_t DOM_PK_F13                  = 0x0064; /* "F13"                */
constexpr glfw_scancode_t DOM_PK_F14                  = 0x0065; /* "F14"                */
constexpr glfw_scancode_t DOM_PK_F15                  = 0x0066; /* "F15"                */
constexpr glfw_scancode_t DOM_PK_F16                  = 0x0067; /* "F16"                */
constexpr glfw_scancode_t DOM_PK_F17                  = 0x0068; /* "F17"                */
constexpr glfw_scancode_t DOM_PK_F18                  = 0x0069; /* "F18"                */
constexpr glfw_scancode_t DOM_PK_F19                  = 0x006A; /* "F19"                */
constexpr glfw_scancode_t DOM_PK_F20                  = 0x006B; /* "F20"                */
constexpr glfw_scancode_t DOM_PK_F21                  = 0x006C; /* "F21"                */
constexpr glfw_scancode_t DOM_PK_F22                  = 0x006D; /* "F22"                */
constexpr glfw_scancode_t DOM_PK_F23                  = 0x006E; /* "F23"                */
constexpr glfw_scancode_t DOM_PK_KANA_MODE            = 0x0070; /* "KanaMode"           */
constexpr glfw_scancode_t DOM_PK_LANG_2               = 0x0071; /* "Lang2"              */
constexpr glfw_scancode_t DOM_PK_LANG_1               = 0x0072; /* "Lang1"              */
constexpr glfw_scancode_t DOM_PK_INTL_RO              = 0x0073; /* "IntlRo"             */
constexpr glfw_scancode_t DOM_PK_F24                  = 0x0076; /* "F24"                */
constexpr glfw_scancode_t DOM_PK_CONVERT              = 0x0079; /* "Convert"            */
constexpr glfw_scancode_t DOM_PK_NON_CONVERT          = 0x007B; /* "NonConvert"         */
constexpr glfw_scancode_t DOM_PK_INTL_YEN             = 0x007D; /* "IntlYen"            */
constexpr glfw_scancode_t DOM_PK_NUMPAD_COMMA         = 0x007E; /* "NumpadComma"        */
constexpr glfw_scancode_t DOM_PK_PASTE                = 0xE00A; /* "Paste"              */
constexpr glfw_scancode_t DOM_PK_MEDIA_TRACK_PREVIOUS = 0xE010; /* "MediaTrackPrevious" */
constexpr glfw_scancode_t DOM_PK_CUT                  = 0xE017; /* "Cut"                */
constexpr glfw_scancode_t DOM_PK_COPY                 = 0xE018; /* "Copy"               */
constexpr glfw_scancode_t DOM_PK_MEDIA_TRACK_NEXT     = 0xE019; /* "MediaTrackNext"     */
constexpr glfw_scancode_t DOM_PK_NUMPAD_ENTER         = 0xE01C; /* "NumpadEnter"        */
constexpr glfw_scancode_t DOM_PK_CONTROL_RIGHT        = 0xE01D; /* "ControlRight"       */
constexpr glfw_scancode_t DOM_PK_AUDIO_VOLUME_MUTE    = 0xE020; /* "AudioVolumeMute"    */
// constexpr glfw_scancode_t DOM_PK_AUDIO_VOLUME_MUTE    = 0xE020; /* "VolumeMute"         */
constexpr glfw_scancode_t DOM_PK_LAUNCH_APP_2         = 0xE021; /* "LaunchApp2"         */
constexpr glfw_scancode_t DOM_PK_MEDIA_PLAY_PAUSE     = 0xE022; /* "MediaPlayPause"     */
constexpr glfw_scancode_t DOM_PK_MEDIA_STOP           = 0xE024; /* "MediaStop"          */
constexpr glfw_scancode_t DOM_PK_EJECT                = 0xE02C; /* "Eject"              */
constexpr glfw_scancode_t DOM_PK_AUDIO_VOLUME_DOWN    = 0xE02E; /* "AudioVolumeDown"    */
// constexpr glfw_scancode_t DOM_PK_AUDIO_VOLUME_DOWN    = 0xE02E; /* "VolumeDown"         */
constexpr glfw_scancode_t DOM_PK_AUDIO_VOLUME_UP      = 0xE030; /* "AudioVolumeUp"      */
// constexpr glfw_scancode_t DOM_PK_AUDIO_VOLUME_UP      = 0xE030; /* "VolumeUp"           */
constexpr glfw_scancode_t DOM_PK_BROWSER_HOME         = 0xE032; /* "BrowserHome"        */
constexpr glfw_scancode_t DOM_PK_NUMPAD_DIVIDE        = 0xE035; /* "NumpadDivide"       */
constexpr glfw_scancode_t DOM_PK_ALT_RIGHT            = 0xE038; /* "AltRight"           */
constexpr glfw_scancode_t DOM_PK_HELP                 = 0xE03B; /* "Help"               */
constexpr glfw_scancode_t DOM_PK_NUM_LOCK             = 0xE045; /* "NumLock"            */
constexpr glfw_scancode_t DOM_PK_HOME                 = 0xE047; /* "Home"               */
constexpr glfw_scancode_t DOM_PK_ARROW_UP             = 0xE048; /* "ArrowUp"            */
constexpr glfw_scancode_t DOM_PK_PAGE_UP              = 0xE049; /* "PageUp"             */
constexpr glfw_scancode_t DOM_PK_ARROW_LEFT           = 0xE04B; /* "ArrowLeft"          */
constexpr glfw_scancode_t DOM_PK_ARROW_RIGHT          = 0xE04D; /* "ArrowRight"         */
constexpr glfw_scancode_t DOM_PK_END                  = 0xE04F; /* "End"                */
constexpr glfw_scancode_t DOM_PK_ARROW_DOWN           = 0xE050; /* "ArrowDown"          */
constexpr glfw_scancode_t DOM_PK_PAGE_DOWN            = 0xE051; /* "PageDown"           */
constexpr glfw_scancode_t DOM_PK_INSERT               = 0xE052; /* "Insert"             */
constexpr glfw_scancode_t DOM_PK_DELETE               = 0xE053; /* "Delete"             */
constexpr glfw_scancode_t DOM_PK_META_LEFT            = 0xE05B; /* "MetaLeft"           */
constexpr glfw_scancode_t DOM_PK_OS_LEFT              = 0xE05B; /* "OSLeft"             */
constexpr glfw_scancode_t DOM_PK_META_RIGHT           = 0xE05C; /* "MetaRight"          */
constexpr glfw_scancode_t DOM_PK_OS_RIGHT             = 0xE05C; /* "OSRight"            */
constexpr glfw_scancode_t DOM_PK_CONTEXT_MENU         = 0xE05D; /* "ContextMenu"        */
constexpr glfw_scancode_t DOM_PK_POWER                = 0xE05E; /* "Power"              */
constexpr glfw_scancode_t DOM_PK_BROWSER_SEARCH       = 0xE065; /* "BrowserSearch"      */
constexpr glfw_scancode_t DOM_PK_BROWSER_FAVORITES    = 0xE066; /* "BrowserFavorites"   */
constexpr glfw_scancode_t DOM_PK_BROWSER_REFRESH      = 0xE067; /* "BrowserRefresh"     */
constexpr glfw_scancode_t DOM_PK_BROWSER_STOP         = 0xE068; /* "BrowserStop"        */
constexpr glfw_scancode_t DOM_PK_BROWSER_FORWARD      = 0xE069; /* "BrowserForward"     */
constexpr glfw_scancode_t DOM_PK_BROWSER_BACK         = 0xE06A; /* "BrowserBack"        */
constexpr glfw_scancode_t DOM_PK_LAUNCH_APP_1         = 0xE06B; /* "LaunchApp1"         */
constexpr glfw_scancode_t DOM_PK_LAUNCH_MAIL          = 0xE06C; /* "LaunchMail"         */
constexpr glfw_scancode_t DOM_PK_LAUNCH_MEDIA_PLAYER  = 0xE06D; /* "LaunchMediaPlayer"  */
constexpr glfw_scancode_t DOM_PK_MEDIA_SELECT         = 0xE06D; /* "MediaSelect"        */

//------------------------------------------------------------------------
// keyboardEventCodeToScancode: maps the code coming from keyboardEvent.code 
// (which is a string!) to a unique scancode number 
//------------------------------------------------------------------------
constexpr glfw_scancode_t keyboardEventCodeToScancode(char const *iCode)
{
  if (!iCode) return 0;

  /* Compute the collision free hash. */
  unsigned int hash = 0;
  while(*iCode) hash = ((hash ^ 0x7E057D79U) << 3) ^ (unsigned int)*iCode++;

  switch(hash)
  {
    case 0x98051284U /* Unidentified       */: return DOM_PK_UNKNOWN;              /* 0x0000 */
    case 0x67243A2DU /* Escape             */: return DOM_PK_ESCAPE;               /* 0x0001 */
    case 0x67251058U /* Digit0             */: return DOM_PK_0;                    /* 0x0002 */
    case 0x67251059U /* Digit1             */: return DOM_PK_1;                    /* 0x0003 */
    case 0x6725105AU /* Digit2             */: return DOM_PK_2;                    /* 0x0004 */
    case 0x6725105BU /* Digit3             */: return DOM_PK_3;                    /* 0x0005 */
    case 0x6725105CU /* Digit4             */: return DOM_PK_4;                    /* 0x0006 */
    case 0x6725105DU /* Digit5             */: return DOM_PK_5;                    /* 0x0007 */
    case 0x6725105EU /* Digit6             */: return DOM_PK_6;                    /* 0x0008 */
    case 0x6725105FU /* Digit7             */: return DOM_PK_7;                    /* 0x0009 */
    case 0x67251050U /* Digit8             */: return DOM_PK_8;                    /* 0x000A */
    case 0x67251051U /* Digit9             */: return DOM_PK_9;                    /* 0x000B */
    case 0x92E14DD3U /* Minus              */: return DOM_PK_MINUS;                /* 0x000C */
    case 0x92E1FBACU /* Equal              */: return DOM_PK_EQUAL;                /* 0x000D */
    case 0x36BF1CB5U /* Backspace          */: return DOM_PK_BACKSPACE;            /* 0x000E */
    case 0x7B8E51E2U /* Tab                */: return DOM_PK_TAB;                  /* 0x000F */
    case 0x2C595B51U /* KeyQ               */: return DOM_PK_Q;                    /* 0x0010 */
    case 0x2C595B57U /* KeyW               */: return DOM_PK_W;                    /* 0x0011 */
    case 0x2C595B45U /* KeyE               */: return DOM_PK_E;                    /* 0x0012 */
    case 0x2C595B52U /* KeyR               */: return DOM_PK_R;                    /* 0x0013 */
    case 0x2C595B54U /* KeyT               */: return DOM_PK_T;                    /* 0x0014 */
    case 0x2C595B59U /* KeyY               */: return DOM_PK_Y;                    /* 0x0015 */
    case 0x2C595B55U /* KeyU               */: return DOM_PK_U;                    /* 0x0016 */
    case 0x2C595B49U /* KeyI               */: return DOM_PK_I;                    /* 0x0017 */
    case 0x2C595B4FU /* KeyO               */: return DOM_PK_O;                    /* 0x0018 */
    case 0x2C595B50U /* KeyP               */: return DOM_PK_P;                    /* 0x0019 */
    case 0x45D8158CU /* BracketLeft        */: return DOM_PK_BRACKET_LEFT;         /* 0x001A */
    case 0xDEEABF7CU /* BracketRight       */: return DOM_PK_BRACKET_RIGHT;        /* 0x001B */
    case 0x92E1C5D2U /* Enter              */: return DOM_PK_ENTER;                /* 0x001C */
    case 0xE058958CU /* ControlLeft        */: return DOM_PK_CONTROL_LEFT;         /* 0x001D */
    case 0x2C595B41U /* KeyA               */: return DOM_PK_A;                    /* 0x001E */
    case 0x2C595B53U /* KeyS               */: return DOM_PK_S;                    /* 0x001F */
    case 0x2C595B44U /* KeyD               */: return DOM_PK_D;                    /* 0x0020 */
    case 0x2C595B46U /* KeyF               */: return DOM_PK_F;                    /* 0x0021 */
    case 0x2C595B47U /* KeyG               */: return DOM_PK_G;                    /* 0x0022 */
    case 0x2C595B48U /* KeyH               */: return DOM_PK_H;                    /* 0x0023 */
    case 0x2C595B4AU /* KeyJ               */: return DOM_PK_J;                    /* 0x0024 */
    case 0x2C595B4BU /* KeyK               */: return DOM_PK_K;                    /* 0x0025 */
    case 0x2C595B4CU /* KeyL               */: return DOM_PK_L;                    /* 0x0026 */
    case 0x2707219EU /* Semicolon          */: return DOM_PK_SEMICOLON;            /* 0x0027 */
    case 0x92E0B58DU /* Quote              */: return DOM_PK_QUOTE;                /* 0x0028 */
    case 0x36BF358DU /* Backquote          */: return DOM_PK_BACKQUOTE;            /* 0x0029 */
    case 0x26B1958CU /* ShiftLeft          */: return DOM_PK_SHIFT_LEFT;           /* 0x002A */
    case 0x36BF2438U /* Backslash          */: return DOM_PK_BACKSLASH;            /* 0x002B */
    case 0x2C595B5AU /* KeyZ               */: return DOM_PK_Z;                    /* 0x002C */
    case 0x2C595B58U /* KeyX               */: return DOM_PK_X;                    /* 0x002D */
    case 0x2C595B43U /* KeyC               */: return DOM_PK_C;                    /* 0x002E */
    case 0x2C595B56U /* KeyV               */: return DOM_PK_V;                    /* 0x002F */
    case 0x2C595B42U /* KeyB               */: return DOM_PK_B;                    /* 0x0030 */
    case 0x2C595B4EU /* KeyN               */: return DOM_PK_N;                    /* 0x0031 */
    case 0x2C595B4DU /* KeyM               */: return DOM_PK_M;                    /* 0x0032 */
    case 0x92E1A1C1U /* Comma              */: return DOM_PK_COMMA;                /* 0x0033 */
    case 0x672FFAD4U /* Period             */: return DOM_PK_PERIOD;               /* 0x0034 */
    case 0x92E0A438U /* Slash              */: return DOM_PK_SLASH;                /* 0x0035 */
    case 0xC5A6BF7CU /* ShiftRight         */: return DOM_PK_SHIFT_RIGHT;          /* 0x0036 */
    case 0x5D64DA91U /* NumpadMultiply     */: return DOM_PK_NUMPAD_MULTIPLY;      /* 0x0037 */
    case 0xC914958CU /* AltLeft            */: return DOM_PK_ALT_LEFT;             /* 0x0038 */
    case 0x92E09CB5U /* Space              */: return DOM_PK_SPACE;                /* 0x0039 */
    case 0xB8FAE73BU /* CapsLock           */: return DOM_PK_CAPS_LOCK;            /* 0x003A */
    case 0x7174B789U /* F1                 */: return DOM_PK_F1;                   /* 0x003B */
    case 0x7174B78AU /* F2                 */: return DOM_PK_F2;                   /* 0x003C */
    case 0x7174B78BU /* F3                 */: return DOM_PK_F3;                   /* 0x003D */
    case 0x7174B78CU /* F4                 */: return DOM_PK_F4;                   /* 0x003E */
    case 0x7174B78DU /* F5                 */: return DOM_PK_F5;                   /* 0x003F */
    case 0x7174B78EU /* F6                 */: return DOM_PK_F6;                   /* 0x0040 */
    case 0x7174B78FU /* F7                 */: return DOM_PK_F7;                   /* 0x0041 */
    case 0x7174B780U /* F8                 */: return DOM_PK_F8;                   /* 0x0042 */
    case 0x7174B781U /* F9                 */: return DOM_PK_F9;                   /* 0x0043 */
    case 0x7B8E57B0U /* F10                */: return DOM_PK_F10;                  /* 0x0044 */
    case 0x92E08B35U /* Pause              */: return DOM_PK_PAUSE;                /* 0x0045 */
    case 0xCDED173BU /* ScrollLock         */: return DOM_PK_SCROLL_LOCK;          /* 0x0046 */
    case 0xC925FCDFU /* Numpad7            */: return DOM_PK_NUMPAD_7;             /* 0x0047 */
    case 0xC925FCD0U /* Numpad8            */: return DOM_PK_NUMPAD_8;             /* 0x0048 */
    case 0xC925FCD1U /* Numpad9            */: return DOM_PK_NUMPAD_9;             /* 0x0049 */
    case 0x5EA3E8A4U /* NumpadSubtract     */: return DOM_PK_NUMPAD_SUBTRACT;      /* 0x004A */
    case 0xC925FCDCU /* Numpad4            */: return DOM_PK_NUMPAD_4;             /* 0x004B */
    case 0xC925FCDDU /* Numpad5            */: return DOM_PK_NUMPAD_5;             /* 0x004C */
    case 0xC925FCDEU /* Numpad6            */: return DOM_PK_NUMPAD_6;             /* 0x004D */
    case 0x380B9C8CU /* NumpadAdd          */: return DOM_PK_NUMPAD_ADD;           /* 0x004E */
    case 0xC925FCD9U /* Numpad1            */: return DOM_PK_NUMPAD_1;             /* 0x004F */
    case 0xC925FCDAU /* Numpad2            */: return DOM_PK_NUMPAD_2;             /* 0x0050 */
    case 0xC925FCDBU /* Numpad3            */: return DOM_PK_NUMPAD_3;             /* 0x0051 */
    case 0xC925FCD8U /* Numpad0            */: return DOM_PK_NUMPAD_0;             /* 0x0052 */
    case 0x95852DACU /* NumpadDecimal      */: return DOM_PK_NUMPAD_DECIMAL;       /* 0x0053 */
    case 0xCC1E198EU /* PrintScreen        */: return DOM_PK_PRINT_SCREEN;         /* 0x0054 */
    case 0x16BF2438U /* IntlBackslash      */: return DOM_PK_INTL_BACKSLASH;       /* 0x0056 */
    case 0x7B8E57B1U /* F11                */: return DOM_PK_F11;                  /* 0x0057 */
    case 0x7B8E57B2U /* F12                */: return DOM_PK_F12;                  /* 0x0058 */
    case 0x7393FBACU /* NumpadEqual        */: return DOM_PK_NUMPAD_EQUAL;         /* 0x0059 */
    case 0x7B8E57B3U /* F13                */: return DOM_PK_F13;                  /* 0x0064 */
    case 0x7B8E57B4U /* F14                */: return DOM_PK_F14;                  /* 0x0065 */
    case 0x7B8E57B5U /* F15                */: return DOM_PK_F15;                  /* 0x0066 */
    case 0x7B8E57B6U /* F16                */: return DOM_PK_F16;                  /* 0x0067 */
    case 0x7B8E57B7U /* F17                */: return DOM_PK_F17;                  /* 0x0068 */
    case 0x7B8E57B8U /* F18                */: return DOM_PK_F18;                  /* 0x0069 */
    case 0x7B8E57B9U /* F19                */: return DOM_PK_F19;                  /* 0x006A */
    case 0x7B8E57A8U /* F20                */: return DOM_PK_F20;                  /* 0x006B */
    case 0x7B8E57A9U /* F21                */: return DOM_PK_F21;                  /* 0x006C */
    case 0x7B8E57AAU /* F22                */: return DOM_PK_F22;                  /* 0x006D */
    case 0x7B8E57ABU /* F23                */: return DOM_PK_F23;                  /* 0x006E */
    case 0xB9F4C50DU /* KanaMode           */: return DOM_PK_KANA_MODE;            /* 0x0070 */
    case 0x92E14D02U /* Lang2              */: return DOM_PK_LANG_2;               /* 0x0071 */
    case 0x92E14D01U /* Lang1              */: return DOM_PK_LANG_1;               /* 0x0072 */
    case 0x6723C677U /* IntlRo             */: return DOM_PK_INTL_RO;              /* 0x0073 */
    case 0x7B8E57ACU /* F24                */: return DOM_PK_F24;                  /* 0x0076 */
    case 0xC91CC12CU /* Convert            */: return DOM_PK_CONVERT;              /* 0x0079 */
    case 0x2ADCC12CU /* NonConvert         */: return DOM_PK_NON_CONVERT;          /* 0x007B */
    case 0xC935DA8EU /* IntlYen            */: return DOM_PK_INTL_YEN;             /* 0x007D */
    case 0x7393A1C1U /* NumpadComma        */: return DOM_PK_NUMPAD_COMMA;         /* 0x007E */
    case 0x92E08A8DU /* Paste              */: return DOM_PK_PASTE;                /* 0xE00A */
    case 0x01DC7D93U /* MediaTrackPrevious */: return DOM_PK_MEDIA_TRACK_PREVIOUS; /* 0xE010 */
    case 0x7B8E5494U /* Cut                */: return DOM_PK_CUT;                  /* 0xE017 */
    case 0x2C5949B1U /* Copy               */: return DOM_PK_COPY;                 /* 0xE018 */
    case 0x2AD2E17CU /* MediaTrackNext     */: return DOM_PK_MEDIA_TRACK_NEXT;     /* 0xE019 */
    case 0x7393C5D2U /* NumpadEnter        */: return DOM_PK_NUMPAD_ENTER;         /* 0xE01C */
    case 0xF2EEBF7CU /* ControlRight       */: return DOM_PK_CONTROL_RIGHT;        /* 0xE01D */
    case 0x2A45030DU /* AudioVolumeMute    */: return DOM_PK_AUDIO_VOLUME_MUTE;    /* 0xE020 */
    case 0xEA45030DU /* VolumeMute         */: return DOM_PK_AUDIO_VOLUME_MUTE;    /* 0xE020 */
    case 0x370ECA3AU /* LaunchApp2         */: return DOM_PK_LAUNCH_APP_2;         /* 0xE021 */
    case 0x2D1C0B35U /* MediaPlayPause     */: return DOM_PK_MEDIA_PLAY_PAUSE;     /* 0xE022 */
    case 0x39237F80U /* MediaStop          */: return DOM_PK_MEDIA_STOP;           /* 0xE024 */
    case 0x92E1C9A4U /* Eject              */: return DOM_PK_EJECT;                /* 0xE02C */
    case 0x2A45179EU /* AudioVolumeDown    */: return DOM_PK_AUDIO_VOLUME_DOWN;    /* 0xE02E */
    case 0xEA45179EU /* VolumeDown         */: return DOM_PK_AUDIO_VOLUME_DOWN;    /* 0xE02E */
    case 0x156CC610U /* AudioVolumeUp      */: return DOM_PK_AUDIO_VOLUME_UP;      /* 0xE030 */
    case 0xBA6CC610U /* VolumeUp           */: return DOM_PK_AUDIO_VOLUME_UP;      /* 0xE030 */
    case 0x49387F45U /* BrowserHome        */: return DOM_PK_BROWSER_HOME;         /* 0xE032 */
    case 0x6CB5328DU /* NumpadDivide       */: return DOM_PK_NUMPAD_DIVIDE;        /* 0xE035 */
    case 0xB88EBF7CU /* AltRight           */: return DOM_PK_ALT_RIGHT;            /* 0xE038 */
    case 0x2C595DD8U /* Help               */: return DOM_PK_HELP;                 /* 0xE03B */
    case 0xC925873BU /* NumLock            */: return DOM_PK_NUM_LOCK;             /* 0xE045 */
    case 0x2C595F45U /* Home               */: return DOM_PK_HOME;                 /* 0xE047 */
    case 0xC91BB690U /* ArrowUp            */: return DOM_PK_ARROW_UP;             /* 0xE048 */
    case 0x672F9210U /* PageUp             */: return DOM_PK_PAGE_UP;              /* 0xE049 */
    case 0x3799258CU /* ArrowLeft          */: return DOM_PK_ARROW_LEFT;           /* 0xE04B */
    case 0x4CE33F7CU /* ArrowRight         */: return DOM_PK_ARROW_RIGHT;          /* 0xE04D */
    case 0x7B8E55DCU /* End                */: return DOM_PK_END;                  /* 0xE04F */
    case 0x3799379EU /* ArrowDown          */: return DOM_PK_ARROW_DOWN;           /* 0xE050 */
    case 0xBA90179EU /* PageDown           */: return DOM_PK_PAGE_DOWN;            /* 0xE051 */
    case 0x6723CB2CU /* Insert             */: return DOM_PK_INSERT;               /* 0xE052 */
    case 0x6725C50DU /* Delete             */: return DOM_PK_DELETE;               /* 0xE053 */
    case 0xB929C58CU /* MetaLeft           */: return DOM_PK_META_LEFT;            /* 0xE05B */
    case 0x6723658CU /* OSLeft             */: return DOM_PK_OS_LEFT;              /* 0xE05B */
    case 0x39643F7CU /* MetaRight          */: return DOM_PK_META_RIGHT;           /* 0xE05C */
    case 0xC9313F7CU /* OSRight            */: return DOM_PK_OS_RIGHT;             /* 0xE05C */
    case 0xE00E97CDU /* ContextMenu        */: return DOM_PK_CONTEXT_MENU;         /* 0xE05D */
    case 0x92E09712U /* Power              */: return DOM_PK_POWER;                /* 0xE05E */
    case 0x3F665A78U /* BrowserSearch      */: return DOM_PK_BROWSER_SEARCH;       /* 0xE065 */
    case 0xA2E93BD3U /* BrowserFavorites   */: return DOM_PK_BROWSER_FAVORITES;    /* 0xE066 */
    case 0x0B1D4938U /* BrowserRefresh     */: return DOM_PK_BROWSER_REFRESH;      /* 0xE067 */
    case 0x49384F80U /* BrowserStop        */: return DOM_PK_BROWSER_STOP;         /* 0xE068 */
    case 0x0B49023CU /* BrowserForward     */: return DOM_PK_BROWSER_FORWARD;      /* 0xE069 */
    case 0x493868BBU /* BrowserBack        */: return DOM_PK_BROWSER_BACK;         /* 0xE06A */
    case 0x370ECA39U /* LaunchApp1         */: return DOM_PK_LAUNCH_APP_1;         /* 0xE06B */
    case 0x370ED6ECU /* LaunchMail         */: return DOM_PK_LAUNCH_MAIL;          /* 0xE06C */
    case 0x39AB4892U /* LaunchMediaPlayer  */: return DOM_PK_LAUNCH_MEDIA_PLAYER;  /* 0xE06D */
    case 0x39AA45A4U /* MediaSelect        */: return DOM_PK_MEDIA_SELECT;         /* 0xE06D */
    default: return DOM_PK_UNKNOWN;
  }
}


//------------------------------------------------------------------------
// scancodeToString: maps glfw_scancode_t to string
// returns nullptr when no mapping
//------------------------------------------------------------------------
constexpr char const *scancodeToString(glfw_scancode_t iCode)
{
  switch(iCode)
  {
    case DOM_PK_UNKNOWN:              return "DOM_PK_UNKNOWN";
    case DOM_PK_ESCAPE:               return "DOM_PK_ESCAPE";
    case DOM_PK_0:                    return "DOM_PK_0";
    case DOM_PK_1:                    return "DOM_PK_1";
    case DOM_PK_2:                    return "DOM_PK_2";
    case DOM_PK_3:                    return "DOM_PK_3";
    case DOM_PK_4:                    return "DOM_PK_4";
    case DOM_PK_5:                    return "DOM_PK_5";
    case DOM_PK_6:                    return "DOM_PK_6";
    case DOM_PK_7:                    return "DOM_PK_7";
    case DOM_PK_8:                    return "DOM_PK_8";
    case DOM_PK_9:                    return "DOM_PK_9";
    case DOM_PK_MINUS:                return "DOM_PK_MINUS";
    case DOM_PK_EQUAL:                return "DOM_PK_EQUAL";
    case DOM_PK_BACKSPACE:            return "DOM_PK_BACKSPACE";
    case DOM_PK_TAB:                  return "DOM_PK_TAB";
    case DOM_PK_Q:                    return "DOM_PK_Q";
    case DOM_PK_W:                    return "DOM_PK_W";
    case DOM_PK_E:                    return "DOM_PK_E";
    case DOM_PK_R:                    return "DOM_PK_R";
    case DOM_PK_T:                    return "DOM_PK_T";
    case DOM_PK_Y:                    return "DOM_PK_Y";
    case DOM_PK_U:                    return "DOM_PK_U";
    case DOM_PK_I:                    return "DOM_PK_I";
    case DOM_PK_O:                    return "DOM_PK_O";
    case DOM_PK_P:                    return "DOM_PK_P";
    case DOM_PK_BRACKET_LEFT:         return "DOM_PK_BRACKET_LEFT";
    case DOM_PK_BRACKET_RIGHT:        return "DOM_PK_BRACKET_RIGHT";
    case DOM_PK_ENTER:                return "DOM_PK_ENTER";
    case DOM_PK_CONTROL_LEFT:         return "DOM_PK_CONTROL_LEFT";
    case DOM_PK_A:                    return "DOM_PK_A";
    case DOM_PK_S:                    return "DOM_PK_S";
    case DOM_PK_D:                    return "DOM_PK_D";
    case DOM_PK_F:                    return "DOM_PK_F";
    case DOM_PK_G:                    return "DOM_PK_G";
    case DOM_PK_H:                    return "DOM_PK_H";
    case DOM_PK_J:                    return "DOM_PK_J";
    case DOM_PK_K:                    return "DOM_PK_K";
    case DOM_PK_L:                    return "DOM_PK_L";
    case DOM_PK_SEMICOLON:            return "DOM_PK_SEMICOLON";
    case DOM_PK_QUOTE:                return "DOM_PK_QUOTE";
    case DOM_PK_BACKQUOTE:            return "DOM_PK_BACKQUOTE";
    case DOM_PK_SHIFT_LEFT:           return "DOM_PK_SHIFT_LEFT";
    case DOM_PK_BACKSLASH:            return "DOM_PK_BACKSLASH";
    case DOM_PK_Z:                    return "DOM_PK_Z";
    case DOM_PK_X:                    return "DOM_PK_X";
    case DOM_PK_C:                    return "DOM_PK_C";
    case DOM_PK_V:                    return "DOM_PK_V";
    case DOM_PK_B:                    return "DOM_PK_B";
    case DOM_PK_N:                    return "DOM_PK_N";
    case DOM_PK_M:                    return "DOM_PK_M";
    case DOM_PK_COMMA:                return "DOM_PK_COMMA";
    case DOM_PK_PERIOD:               return "DOM_PK_PERIOD";
    case DOM_PK_SLASH:                return "DOM_PK_SLASH";
    case DOM_PK_SHIFT_RIGHT:          return "DOM_PK_SHIFT_RIGHT";
    case DOM_PK_NUMPAD_MULTIPLY:      return "DOM_PK_NUMPAD_MULTIPLY";
    case DOM_PK_ALT_LEFT:             return "DOM_PK_ALT_LEFT";
    case DOM_PK_SPACE:                return "DOM_PK_SPACE";
    case DOM_PK_CAPS_LOCK:            return "DOM_PK_CAPS_LOCK";
    case DOM_PK_F1:                   return "DOM_PK_F1";
    case DOM_PK_F2:                   return "DOM_PK_F2";
    case DOM_PK_F3:                   return "DOM_PK_F3";
    case DOM_PK_F4:                   return "DOM_PK_F4";
    case DOM_PK_F5:                   return "DOM_PK_F5";
    case DOM_PK_F6:                   return "DOM_PK_F6";
    case DOM_PK_F7:                   return "DOM_PK_F7";
    case DOM_PK_F8:                   return "DOM_PK_F8";
    case DOM_PK_F9:                   return "DOM_PK_F9";
    case DOM_PK_F10:                  return "DOM_PK_F10";
    case DOM_PK_PAUSE:                return "DOM_PK_PAUSE";
    case DOM_PK_SCROLL_LOCK:          return "DOM_PK_SCROLL_LOCK";
    case DOM_PK_NUMPAD_7:             return "DOM_PK_NUMPAD_7";
    case DOM_PK_NUMPAD_8:             return "DOM_PK_NUMPAD_8";
    case DOM_PK_NUMPAD_9:             return "DOM_PK_NUMPAD_9";
    case DOM_PK_NUMPAD_SUBTRACT:      return "DOM_PK_NUMPAD_SUBTRACT";
    case DOM_PK_NUMPAD_4:             return "DOM_PK_NUMPAD_4";
    case DOM_PK_NUMPAD_5:             return "DOM_PK_NUMPAD_5";
    case DOM_PK_NUMPAD_6:             return "DOM_PK_NUMPAD_6";
    case DOM_PK_NUMPAD_ADD:           return "DOM_PK_NUMPAD_ADD";
    case DOM_PK_NUMPAD_1:             return "DOM_PK_NUMPAD_1";
    case DOM_PK_NUMPAD_2:             return "DOM_PK_NUMPAD_2";
    case DOM_PK_NUMPAD_3:             return "DOM_PK_NUMPAD_3";
    case DOM_PK_NUMPAD_0:             return "DOM_PK_NUMPAD_0";
    case DOM_PK_NUMPAD_DECIMAL:       return "DOM_PK_NUMPAD_DECIMAL";
    case DOM_PK_PRINT_SCREEN:         return "DOM_PK_PRINT_SCREEN";
    case DOM_PK_INTL_BACKSLASH:       return "DOM_PK_INTL_BACKSLASH";
    case DOM_PK_F11:                  return "DOM_PK_F11";
    case DOM_PK_F12:                  return "DOM_PK_F12";
    case DOM_PK_NUMPAD_EQUAL:         return "DOM_PK_NUMPAD_EQUAL";
    case DOM_PK_F13:                  return "DOM_PK_F13";
    case DOM_PK_F14:                  return "DOM_PK_F14";
    case DOM_PK_F15:                  return "DOM_PK_F15";
    case DOM_PK_F16:                  return "DOM_PK_F16";
    case DOM_PK_F17:                  return "DOM_PK_F17";
    case DOM_PK_F18:                  return "DOM_PK_F18";
    case DOM_PK_F19:                  return "DOM_PK_F19";
    case DOM_PK_F20:                  return "DOM_PK_F20";
    case DOM_PK_F21:                  return "DOM_PK_F21";
    case DOM_PK_F22:                  return "DOM_PK_F22";
    case DOM_PK_F23:                  return "DOM_PK_F23";
    case DOM_PK_KANA_MODE:            return "DOM_PK_KANA_MODE";
    case DOM_PK_LANG_2:               return "DOM_PK_LANG_2";
    case DOM_PK_LANG_1:               return "DOM_PK_LANG_1";
    case DOM_PK_INTL_RO:              return "DOM_PK_INTL_RO";
    case DOM_PK_F24:                  return "DOM_PK_F24";
    case DOM_PK_CONVERT:              return "DOM_PK_CONVERT";
    case DOM_PK_NON_CONVERT:          return "DOM_PK_NON_CONVERT";
    case DOM_PK_INTL_YEN:             return "DOM_PK_INTL_YEN";
    case DOM_PK_NUMPAD_COMMA:         return "DOM_PK_NUMPAD_COMMA";
    case DOM_PK_PASTE:                return "DOM_PK_PASTE";
    case DOM_PK_MEDIA_TRACK_PREVIOUS: return "DOM_PK_MEDIA_TRACK_PREVIOUS";
    case DOM_PK_CUT:                  return "DOM_PK_CUT";
    case DOM_PK_COPY:                 return "DOM_PK_COPY";
    case DOM_PK_MEDIA_TRACK_NEXT:     return "DOM_PK_MEDIA_TRACK_NEXT";
    case DOM_PK_NUMPAD_ENTER:         return "DOM_PK_NUMPAD_ENTER";
    case DOM_PK_CONTROL_RIGHT:        return "DOM_PK_CONTROL_RIGHT";
    case DOM_PK_AUDIO_VOLUME_MUTE:    return "DOM_PK_AUDIO_VOLUME_MUTE";
    case DOM_PK_LAUNCH_APP_2:         return "DOM_PK_LAUNCH_APP_2";
    case DOM_PK_MEDIA_PLAY_PAUSE:     return "DOM_PK_MEDIA_PLAY_PAUSE";
    case DOM_PK_MEDIA_STOP:           return "DOM_PK_MEDIA_STOP";
    case DOM_PK_EJECT:                return "DOM_PK_EJECT";
    case DOM_PK_AUDIO_VOLUME_DOWN:    return "DOM_PK_AUDIO_VOLUME_DOWN";
    case DOM_PK_AUDIO_VOLUME_UP:      return "DOM_PK_AUDIO_VOLUME_UP";
    case DOM_PK_BROWSER_HOME:         return "DOM_PK_BROWSER_HOME";
    case DOM_PK_NUMPAD_DIVIDE:        return "DOM_PK_NUMPAD_DIVIDE";
    case DOM_PK_ALT_RIGHT:            return "DOM_PK_ALT_RIGHT";
    case DOM_PK_HELP:                 return "DOM_PK_HELP";
    case DOM_PK_NUM_LOCK:             return "DOM_PK_NUM_LOCK";
    case DOM_PK_HOME:                 return "DOM_PK_HOME";
    case DOM_PK_ARROW_UP:             return "DOM_PK_ARROW_UP";
    case DOM_PK_PAGE_UP:              return "DOM_PK_PAGE_UP";
    case DOM_PK_ARROW_LEFT:           return "DOM_PK_ARROW_LEFT";
    case DOM_PK_ARROW_RIGHT:          return "DOM_PK_ARROW_RIGHT";
    case DOM_PK_END:                  return "DOM_PK_END";
    case DOM_PK_ARROW_DOWN:           return "DOM_PK_ARROW_DOWN";
    case DOM_PK_PAGE_DOWN:            return "DOM_PK_PAGE_DOWN";
    case DOM_PK_INSERT:               return "DOM_PK_INSERT";
    case DOM_PK_DELETE:               return "DOM_PK_DELETE";
    case DOM_PK_META_LEFT:            return "DOM_PK_META_LEFT";
    case DOM_PK_META_RIGHT:           return "DOM_PK_META_RIGHT";
    case DOM_PK_CONTEXT_MENU:         return "DOM_PK_CONTEXT_MENU";
    case DOM_PK_POWER:                return "DOM_PK_POWER";
    case DOM_PK_BROWSER_SEARCH:       return "DOM_PK_BROWSER_SEARCH";
    case DOM_PK_BROWSER_FAVORITES:    return "DOM_PK_BROWSER_FAVORITES";
    case DOM_PK_BROWSER_REFRESH:      return "DOM_PK_BROWSER_REFRESH";
    case DOM_PK_BROWSER_STOP:         return "DOM_PK_BROWSER_STOP";
    case DOM_PK_BROWSER_FORWARD:      return "DOM_PK_BROWSER_FORWARD";
    case DOM_PK_BROWSER_BACK:         return "DOM_PK_BROWSER_BACK";
    case DOM_PK_LAUNCH_APP_1:         return "DOM_PK_LAUNCH_APP_1";
    case DOM_PK_LAUNCH_MAIL:          return "DOM_PK_LAUNCH_MAIL";
    case DOM_PK_LAUNCH_MEDIA_PLAYER:  return "DOM_PK_LAUNCH_MEDIA_PLAYER";
    default: return nullptr;
  }
}


//------------------------------------------------------------------------
// scancodeToKeyCode: maps glfw_scancode_t to the glfw key code
// returns GLFW_KEY_UNKNOWN when not match
//------------------------------------------------------------------------
constexpr glfw_key_t scancodeToKeyCode(glfw_scancode_t iCode)
{
  switch(iCode)
  {
    case DOM_PK_UNKNOWN:              return GLFW_KEY_UNKNOWN;
    case DOM_PK_ESCAPE:               return GLFW_KEY_ESCAPE;
    case DOM_PK_0:                    return GLFW_KEY_0;
    case DOM_PK_1:                    return GLFW_KEY_1;
    case DOM_PK_2:                    return GLFW_KEY_2;
    case DOM_PK_3:                    return GLFW_KEY_3;
    case DOM_PK_4:                    return GLFW_KEY_4;
    case DOM_PK_5:                    return GLFW_KEY_5;
    case DOM_PK_6:                    return GLFW_KEY_6;
    case DOM_PK_7:                    return GLFW_KEY_7;
    case DOM_PK_8:                    return GLFW_KEY_8;
    case DOM_PK_9:                    return GLFW_KEY_9;
    case DOM_PK_MINUS:                return GLFW_KEY_MINUS;
    case DOM_PK_EQUAL:                return GLFW_KEY_EQUAL;
    case DOM_PK_BACKSPACE:            return GLFW_KEY_BACKSPACE;
    case DOM_PK_TAB:                  return GLFW_KEY_TAB;
    case DOM_PK_Q:                    return GLFW_KEY_Q;
    case DOM_PK_W:                    return GLFW_KEY_W;
    case DOM_PK_E:                    return GLFW_KEY_E;
    case DOM_PK_R:                    return GLFW_KEY_R;
    case DOM_PK_T:                    return GLFW_KEY_T;
    case DOM_PK_Y:                    return GLFW_KEY_Y;
    case DOM_PK_U:                    return GLFW_KEY_U;
    case DOM_PK_I:                    return GLFW_KEY_I;
    case DOM_PK_O:                    return GLFW_KEY_O;
    case DOM_PK_P:                    return GLFW_KEY_P;
    case DOM_PK_BRACKET_LEFT:         return GLFW_KEY_LEFT_BRACKET;
    case DOM_PK_BRACKET_RIGHT:        return GLFW_KEY_RIGHT_BRACKET;
    case DOM_PK_ENTER:                return GLFW_KEY_ENTER;
    case DOM_PK_CONTROL_LEFT:         return GLFW_KEY_LEFT_CONTROL;
    case DOM_PK_A:                    return GLFW_KEY_A;
    case DOM_PK_S:                    return GLFW_KEY_S;
    case DOM_PK_D:                    return GLFW_KEY_D;
    case DOM_PK_F:                    return GLFW_KEY_F;
    case DOM_PK_G:                    return GLFW_KEY_G;
    case DOM_PK_H:                    return GLFW_KEY_H;
    case DOM_PK_J:                    return GLFW_KEY_J;
    case DOM_PK_K:                    return GLFW_KEY_K;
    case DOM_PK_L:                    return GLFW_KEY_L;
    case DOM_PK_SEMICOLON:            return GLFW_KEY_SEMICOLON;
    case DOM_PK_QUOTE:                return GLFW_KEY_APOSTROPHE;
    case DOM_PK_BACKQUOTE:            return GLFW_KEY_GRAVE_ACCENT;
    case DOM_PK_SHIFT_LEFT:           return GLFW_KEY_LEFT_SHIFT;
    case DOM_PK_BACKSLASH:            return GLFW_KEY_BACKSLASH;
    case DOM_PK_Z:                    return GLFW_KEY_Z;
    case DOM_PK_X:                    return GLFW_KEY_X;
    case DOM_PK_C:                    return GLFW_KEY_C;
    case DOM_PK_V:                    return GLFW_KEY_V;
    case DOM_PK_B:                    return GLFW_KEY_B;
    case DOM_PK_N:                    return GLFW_KEY_N;
    case DOM_PK_M:                    return GLFW_KEY_M;
    case DOM_PK_COMMA:                return GLFW_KEY_COMMA;
    case DOM_PK_PERIOD:               return GLFW_KEY_PERIOD;
    case DOM_PK_SLASH:                return GLFW_KEY_SLASH;
    case DOM_PK_SHIFT_RIGHT:          return GLFW_KEY_RIGHT_SHIFT;
    case DOM_PK_NUMPAD_MULTIPLY:      return GLFW_KEY_KP_MULTIPLY;
    case DOM_PK_ALT_LEFT:             return GLFW_KEY_LEFT_ALT;
    case DOM_PK_SPACE:                return GLFW_KEY_SPACE;
    case DOM_PK_CAPS_LOCK:            return GLFW_KEY_CAPS_LOCK;
    case DOM_PK_F1:                   return GLFW_KEY_F1;
    case DOM_PK_F2:                   return GLFW_KEY_F2;
    case DOM_PK_F3:                   return GLFW_KEY_F3;
    case DOM_PK_F4:                   return GLFW_KEY_F4;
    case DOM_PK_F5:                   return GLFW_KEY_F5;
    case DOM_PK_F6:                   return GLFW_KEY_F6;
    case DOM_PK_F7:                   return GLFW_KEY_F7;
    case DOM_PK_F8:                   return GLFW_KEY_F8;
    case DOM_PK_F9:                   return GLFW_KEY_F9;
    case DOM_PK_F10:                  return GLFW_KEY_F10;
    case DOM_PK_PAUSE:                return GLFW_KEY_PAUSE;
    case DOM_PK_SCROLL_LOCK:          return GLFW_KEY_SCROLL_LOCK;
    case DOM_PK_NUMPAD_7:             return GLFW_KEY_KP_7;
    case DOM_PK_NUMPAD_8:             return GLFW_KEY_KP_8;
    case DOM_PK_NUMPAD_9:             return GLFW_KEY_KP_9;
    case DOM_PK_NUMPAD_SUBTRACT:      return GLFW_KEY_KP_SUBTRACT;
    case DOM_PK_NUMPAD_4:             return GLFW_KEY_KP_4;
    case DOM_PK_NUMPAD_5:             return GLFW_KEY_KP_5;
    case DOM_PK_NUMPAD_6:             return GLFW_KEY_KP_6;
    case DOM_PK_NUMPAD_ADD:           return GLFW_KEY_KP_ADD;
    case DOM_PK_NUMPAD_1:             return GLFW_KEY_KP_1;
    case DOM_PK_NUMPAD_2:             return GLFW_KEY_KP_2;
    case DOM_PK_NUMPAD_3:             return GLFW_KEY_KP_3;
    case DOM_PK_NUMPAD_0:             return GLFW_KEY_KP_0;
    case DOM_PK_NUMPAD_DECIMAL:       return GLFW_KEY_KP_DECIMAL;
    case DOM_PK_PRINT_SCREEN:         return GLFW_KEY_PRINT_SCREEN;
    case DOM_PK_F11:                  return GLFW_KEY_F11;
    case DOM_PK_F12:                  return GLFW_KEY_F12;
    case DOM_PK_NUMPAD_EQUAL:         return GLFW_KEY_KP_EQUAL;
    case DOM_PK_F13:                  return GLFW_KEY_F13;
    case DOM_PK_F14:                  return GLFW_KEY_F14;
    case DOM_PK_F15:                  return GLFW_KEY_F15;
    case DOM_PK_F16:                  return GLFW_KEY_F16;
    case DOM_PK_F17:                  return GLFW_KEY_F17;
    case DOM_PK_F18:                  return GLFW_KEY_F18;
    case DOM_PK_F19:                  return GLFW_KEY_F19;
    case DOM_PK_F20:                  return GLFW_KEY_F20;
    case DOM_PK_F21:                  return GLFW_KEY_F21;
    case DOM_PK_F22:                  return GLFW_KEY_F22;
    case DOM_PK_F23:                  return GLFW_KEY_F23;
    case DOM_PK_F24:                  return GLFW_KEY_F24;
    case DOM_PK_NUMPAD_ENTER:         return GLFW_KEY_KP_ENTER;
    case DOM_PK_CONTROL_RIGHT:        return GLFW_KEY_RIGHT_CONTROL;
    case DOM_PK_NUMPAD_DIVIDE:        return GLFW_KEY_KP_DIVIDE;
    case DOM_PK_ALT_RIGHT:            return GLFW_KEY_RIGHT_ALT;
    case DOM_PK_NUM_LOCK:             return GLFW_KEY_NUM_LOCK;
    case DOM_PK_HOME:                 return GLFW_KEY_HOME;
    case DOM_PK_ARROW_UP:             return GLFW_KEY_UP;
    case DOM_PK_PAGE_UP:              return GLFW_KEY_PAGE_UP;
    case DOM_PK_ARROW_LEFT:           return GLFW_KEY_LEFT;
    case DOM_PK_ARROW_RIGHT:          return GLFW_KEY_RIGHT;
    case DOM_PK_END:                  return GLFW_KEY_END;
    case DOM_PK_ARROW_DOWN:           return GLFW_KEY_DOWN;
    case DOM_PK_PAGE_DOWN:            return GLFW_KEY_PAGE_DOWN;
    case DOM_PK_INSERT:               return GLFW_KEY_INSERT;
    case DOM_PK_DELETE:               return GLFW_KEY_DELETE;
    case DOM_PK_META_LEFT:            return GLFW_KEY_LEFT_SUPER;
    case DOM_PK_META_RIGHT:           return GLFW_KEY_RIGHT_SUPER;
    case DOM_PK_CONTEXT_MENU:         return GLFW_KEY_MENU;
    default: return GLFW_KEY_UNKNOWN;
  }
}


//------------------------------------------------------------------------
// keyCodeToScancode: maps glfw key code to glfw_scancode_t
// returns DOM_PK_UNKNOWN when not match
//------------------------------------------------------------------------
constexpr glfw_scancode_t keyCodeToScancode(glfw_key_t iCode)
{
  switch(iCode)
  {
    case GLFW_KEY_UNKNOWN:       return DOM_PK_UNKNOWN;
    case GLFW_KEY_ESCAPE:        return DOM_PK_ESCAPE;
    case GLFW_KEY_0:             return DOM_PK_0;
    case GLFW_KEY_1:             return DOM_PK_1;
    case GLFW_KEY_2:             return DOM_PK_2;
    case GLFW_KEY_3:             return DOM_PK_3;
    case GLFW_KEY_4:             return DOM_PK_4;
    case GLFW_KEY_5:             return DOM_PK_5;
    case GLFW_KEY_6:             return DOM_PK_6;
    case GLFW_KEY_7:             return DOM_PK_7;
    case GLFW_KEY_8:             return DOM_PK_8;
    case GLFW_KEY_9:             return DOM_PK_9;
    case GLFW_KEY_MINUS:         return DOM_PK_MINUS;
    case GLFW_KEY_EQUAL:         return DOM_PK_EQUAL;
    case GLFW_KEY_BACKSPACE:     return DOM_PK_BACKSPACE;
    case GLFW_KEY_TAB:           return DOM_PK_TAB;
    case GLFW_KEY_Q:             return DOM_PK_Q;
    case GLFW_KEY_W:             return DOM_PK_W;
    case GLFW_KEY_E:             return DOM_PK_E;
    case GLFW_KEY_R:             return DOM_PK_R;
    case GLFW_KEY_T:             return DOM_PK_T;
    case GLFW_KEY_Y:             return DOM_PK_Y;
    case GLFW_KEY_U:             return DOM_PK_U;
    case GLFW_KEY_I:             return DOM_PK_I;
    case GLFW_KEY_O:             return DOM_PK_O;
    case GLFW_KEY_P:             return DOM_PK_P;
    case GLFW_KEY_LEFT_BRACKET:  return DOM_PK_BRACKET_LEFT;
    case GLFW_KEY_RIGHT_BRACKET: return DOM_PK_BRACKET_RIGHT;
    case GLFW_KEY_ENTER:         return DOM_PK_ENTER;
    case GLFW_KEY_LEFT_CONTROL:  return DOM_PK_CONTROL_LEFT;
    case GLFW_KEY_A:             return DOM_PK_A;
    case GLFW_KEY_S:             return DOM_PK_S;
    case GLFW_KEY_D:             return DOM_PK_D;
    case GLFW_KEY_F:             return DOM_PK_F;
    case GLFW_KEY_G:             return DOM_PK_G;
    case GLFW_KEY_H:             return DOM_PK_H;
    case GLFW_KEY_J:             return DOM_PK_J;
    case GLFW_KEY_K:             return DOM_PK_K;
    case GLFW_KEY_L:             return DOM_PK_L;
    case GLFW_KEY_SEMICOLON:     return DOM_PK_SEMICOLON;
    case GLFW_KEY_APOSTROPHE:    return DOM_PK_QUOTE;
    case GLFW_KEY_GRAVE_ACCENT:  return DOM_PK_BACKQUOTE;
    case GLFW_KEY_LEFT_SHIFT:    return DOM_PK_SHIFT_LEFT;
    case GLFW_KEY_BACKSLASH:     return DOM_PK_BACKSLASH;
    case GLFW_KEY_Z:             return DOM_PK_Z;
    case GLFW_KEY_X:             return DOM_PK_X;
    case GLFW_KEY_C:             return DOM_PK_C;
    case GLFW_KEY_V:             return DOM_PK_V;
    case GLFW_KEY_B:             return DOM_PK_B;
    case GLFW_KEY_N:             return DOM_PK_N;
    case GLFW_KEY_M:             return DOM_PK_M;
    case GLFW_KEY_COMMA:         return DOM_PK_COMMA;
    case GLFW_KEY_PERIOD:        return DOM_PK_PERIOD;
    case GLFW_KEY_SLASH:         return DOM_PK_SLASH;
    case GLFW_KEY_RIGHT_SHIFT:   return DOM_PK_SHIFT_RIGHT;
    case GLFW_KEY_KP_MULTIPLY:   return DOM_PK_NUMPAD_MULTIPLY;
    case GLFW_KEY_LEFT_ALT:      return DOM_PK_ALT_LEFT;
    case GLFW_KEY_SPACE:         return DOM_PK_SPACE;
    case GLFW_KEY_CAPS_LOCK:     return DOM_PK_CAPS_LOCK;
    case GLFW_KEY_F1:            return DOM_PK_F1;
    case GLFW_KEY_F2:            return DOM_PK_F2;
    case GLFW_KEY_F3:            return DOM_PK_F3;
    case GLFW_KEY_F4:            return DOM_PK_F4;
    case GLFW_KEY_F5:            return DOM_PK_F5;
    case GLFW_KEY_F6:            return DOM_PK_F6;
    case GLFW_KEY_F7:            return DOM_PK_F7;
    case GLFW_KEY_F8:            return DOM_PK_F8;
    case GLFW_KEY_F9:            return DOM_PK_F9;
    case GLFW_KEY_F10:           return DOM_PK_F10;
    case GLFW_KEY_PAUSE:         return DOM_PK_PAUSE;
    case GLFW_KEY_SCROLL_LOCK:   return DOM_PK_SCROLL_LOCK;
    case GLFW_KEY_KP_7:          return DOM_PK_NUMPAD_7;
    case GLFW_KEY_KP_8:          return DOM_PK_NUMPAD_8;
    case GLFW_KEY_KP_9:          return DOM_PK_NUMPAD_9;
    case GLFW_KEY_KP_SUBTRACT:   return DOM_PK_NUMPAD_SUBTRACT;
    case GLFW_KEY_KP_4:          return DOM_PK_NUMPAD_4;
    case GLFW_KEY_KP_5:          return DOM_PK_NUMPAD_5;
    case GLFW_KEY_KP_6:          return DOM_PK_NUMPAD_6;
    case GLFW_KEY_KP_ADD:        return DOM_PK_NUMPAD_ADD;
    case GLFW_KEY_KP_1:          return DOM_PK_NUMPAD_1;
    case GLFW_KEY_KP_2:          return DOM_PK_NUMPAD_2;
    case GLFW_KEY_KP_3:          return DOM_PK_NUMPAD_3;
    case GLFW_KEY_KP_0:          return DOM_PK_NUMPAD_0;
    case GLFW_KEY_KP_DECIMAL:    return DOM_PK_NUMPAD_DECIMAL;
    case GLFW_KEY_PRINT_SCREEN:  return DOM_PK_PRINT_SCREEN;
    case GLFW_KEY_F11:           return DOM_PK_F11;
    case GLFW_KEY_F12:           return DOM_PK_F12;
    case GLFW_KEY_KP_EQUAL:      return DOM_PK_NUMPAD_EQUAL;
    case GLFW_KEY_F13:           return DOM_PK_F13;
    case GLFW_KEY_F14:           return DOM_PK_F14;
    case GLFW_KEY_F15:           return DOM_PK_F15;
    case GLFW_KEY_F16:           return DOM_PK_F16;
    case GLFW_KEY_F17:           return DOM_PK_F17;
    case GLFW_KEY_F18:           return DOM_PK_F18;
    case GLFW_KEY_F19:           return DOM_PK_F19;
    case GLFW_KEY_F20:           return DOM_PK_F20;
    case GLFW_KEY_F21:           return DOM_PK_F21;
    case GLFW_KEY_F22:           return DOM_PK_F22;
    case GLFW_KEY_F23:           return DOM_PK_F23;
    case GLFW_KEY_F24:           return DOM_PK_F24;
    case GLFW_KEY_KP_ENTER:      return DOM_PK_NUMPAD_ENTER;
    case GLFW_KEY_RIGHT_CONTROL: return DOM_PK_CONTROL_RIGHT;
    case GLFW_KEY_KP_DIVIDE:     return DOM_PK_NUMPAD_DIVIDE;
    case GLFW_KEY_RIGHT_ALT:     return DOM_PK_ALT_RIGHT;
    case GLFW_KEY_NUM_LOCK:      return DOM_PK_NUM_LOCK;
    case GLFW_KEY_HOME:          return DOM_PK_HOME;
    case GLFW_KEY_UP:            return DOM_PK_ARROW_UP;
    case GLFW_KEY_PAGE_UP:       return DOM_PK_PAGE_UP;
    case GLFW_KEY_LEFT:          return DOM_PK_ARROW_LEFT;
    case GLFW_KEY_RIGHT:         return DOM_PK_ARROW_RIGHT;
    case GLFW_KEY_END:           return DOM_PK_END;
    case GLFW_KEY_DOWN:          return DOM_PK_ARROW_DOWN;
    case GLFW_KEY_PAGE_DOWN:     return DOM_PK_PAGE_DOWN;
    case GLFW_KEY_INSERT:        return DOM_PK_INSERT;
    case GLFW_KEY_DELETE:        return DOM_PK_DELETE;
    case GLFW_KEY_LEFT_SUPER:    return DOM_PK_META_LEFT;
    case GLFW_KEY_RIGHT_SUPER:   return DOM_PK_META_RIGHT;
    case GLFW_KEY_MENU:          return DOM_PK_CONTEXT_MENU;
    default: return DOM_PK_UNKNOWN;
  }
}


} // namespace keyboard
} // namespace emscripten::glfw3

#endif // EMSCRIPTEN_GLFW_KEYBOARD_MAPPING_H
