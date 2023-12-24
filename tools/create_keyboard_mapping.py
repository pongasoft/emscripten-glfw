#!/usr/bin/env python3

# This code is copied from emscripten/toos/create_dom_pk_codes.py and adapted for
# the needs of this project (mapping glfw/dom code). It generates a cpp header only
# file with constexpr implementation and namespace

# Original copyright notice:

# Copyright 2017 The Emscripten Authors.  All rights reserved.
# Emscripten is available under two separate licenses, the MIT license and the
# University of Illinois/NCSA Open Source License.  Both these licenses can be
# found in the LICENSE file.


import sys
import random

input_strings = [
  (0x0, 'Unidentified',          'DOM_PK_UNKNOWN', 'GLFW_KEY_UNKNOWN'),
  (0x1, 'Escape',                'DOM_PK_ESCAPE', 'GLFW_KEY_ESCAPE'),
  (0x2, 'Digit0',                'DOM_PK_0', 'GLFW_KEY_0'),
  (0x3, 'Digit1',                'DOM_PK_1', 'GLFW_KEY_1'),
  (0x4, 'Digit2',                'DOM_PK_2', 'GLFW_KEY_2'),
  (0x5, 'Digit3',                'DOM_PK_3', 'GLFW_KEY_3'),
  (0x6, 'Digit4',                'DOM_PK_4', 'GLFW_KEY_4'),
  (0x7, 'Digit5',                'DOM_PK_5', 'GLFW_KEY_5'),
  (0x8, 'Digit6',                'DOM_PK_6', 'GLFW_KEY_6'),
  (0x9, 'Digit7',                'DOM_PK_7', 'GLFW_KEY_7'),
  (0xA, 'Digit8',                'DOM_PK_8', 'GLFW_KEY_8'),
  (0xB, 'Digit9',                'DOM_PK_9', 'GLFW_KEY_9'),
  (0xC,  'Minus',                'DOM_PK_MINUS', 'GLFW_KEY_MINUS'),
  (0xD,  'Equal',                'DOM_PK_EQUAL', 'GLFW_KEY_EQUAL'),
  (0xE,  'Backspace',            'DOM_PK_BACKSPACE', 'GLFW_KEY_BACKSPACE'),
  (0xF,  'Tab',                  'DOM_PK_TAB', 'GLFW_KEY_TAB'),
  (0x10, 'KeyQ',                 'DOM_PK_Q', 'GLFW_KEY_Q'),
  (0x11, 'KeyW',                 'DOM_PK_W', 'GLFW_KEY_W'),
  (0x12, 'KeyE',                 'DOM_PK_E', 'GLFW_KEY_E'),
  (0x13, 'KeyR',                 'DOM_PK_R', 'GLFW_KEY_R'),
  (0x14, 'KeyT',                 'DOM_PK_T', 'GLFW_KEY_T'),
  (0x15, 'KeyY',                 'DOM_PK_Y', 'GLFW_KEY_Y'),
  (0x16, 'KeyU',                 'DOM_PK_U', 'GLFW_KEY_U'),
  (0x18, 'KeyO',                 'DOM_PK_O', 'GLFW_KEY_O'),
  (0x19, 'KeyP',                 'DOM_PK_P', 'GLFW_KEY_P'),
  (0x1A, 'BracketLeft',          'DOM_PK_BRACKET_LEFT', 'GLFW_KEY_LEFT_BRACKET'),
  (0x1B, 'BracketRight',         'DOM_PK_BRACKET_RIGHT', 'GLFW_KEY_RIGHT_BRACKET'),
  (0x1C, 'Enter',                'DOM_PK_ENTER', 'GLFW_KEY_ENTER'),
  (0x1D, 'ControlLeft',          'DOM_PK_CONTROL_LEFT', 'GLFW_KEY_LEFT_CONTROL'),
  (0x1E, 'KeyA',                 'DOM_PK_A', 'GLFW_KEY_A'),
  (0x1F, 'KeyS',                 'DOM_PK_S', 'GLFW_KEY_S'),
  (0x20, 'KeyD',                 'DOM_PK_D', 'GLFW_KEY_D'),
  (0x21, 'KeyF',                 'DOM_PK_F', 'GLFW_KEY_F'),
  (0x22, 'KeyG',                 'DOM_PK_G', 'GLFW_KEY_G'),
  (0x23, 'KeyH',                 'DOM_PK_H', 'GLFW_KEY_H'),
  (0x24, 'KeyJ',                 'DOM_PK_J', 'GLFW_KEY_J'),
  (0x25, 'KeyK',                 'DOM_PK_K', 'GLFW_KEY_K'),
  (0x26, 'KeyL',                 'DOM_PK_L', 'GLFW_KEY_L'),
  (0x27, 'Semicolon',            'DOM_PK_SEMICOLON', 'GLFW_KEY_SEMICOLON'),
  (0x28, 'Quote',                'DOM_PK_QUOTE', 'GLFW_KEY_APOSTROPHE'),
  (0x29, 'Backquote',            'DOM_PK_BACKQUOTE', 'GLFW_KEY_GRAVE_ACCENT'),
  (0x2A, 'ShiftLeft',            'DOM_PK_SHIFT_LEFT', 'GLFW_KEY_LEFT_SHIFT'),
  (0x2B, 'Backslash',            'DOM_PK_BACKSLASH', 'GLFW_KEY_BACKSLASH'),
  (0x2C, 'KeyZ',                 'DOM_PK_Z', 'GLFW_KEY_Z'),
  (0x2D, 'KeyX',                 'DOM_PK_X', 'GLFW_KEY_X'),
  (0x2E, 'KeyC',                 'DOM_PK_C', 'GLFW_KEY_C'),
  (0x2F, 'KeyV',                 'DOM_PK_V', 'GLFW_KEY_V'),
  (0x30, 'KeyB',                 'DOM_PK_B', 'GLFW_KEY_B'),
  (0x31, 'KeyN',                 'DOM_PK_N', 'GLFW_KEY_N'),
  (0x32, 'KeyM',                 'DOM_PK_M', 'GLFW_KEY_M'),
  (0x33, 'Comma',                'DOM_PK_COMMA', 'GLFW_KEY_COMMA'),
  (0x34, 'Period',               'DOM_PK_PERIOD', 'GLFW_KEY_PERIOD'),
  (0x35, 'Slash',                'DOM_PK_SLASH', 'GLFW_KEY_SLASH'),
  (0x36, 'ShiftRight',           'DOM_PK_SHIFT_RIGHT', 'GLFW_KEY_RIGHT_SHIFT'),
  (0x37, 'NumpadMultiply',       'DOM_PK_NUMPAD_MULTIPLY', 'GLFW_KEY_KP_MULTIPLY'),
  (0x38, 'AltLeft',              'DOM_PK_ALT_LEFT', 'GLFW_KEY_LEFT_ALT'),
  (0x39, 'Space',                'DOM_PK_SPACE', 'GLFW_KEY_SPACE'),
  (0x3A, 'CapsLock',             'DOM_PK_CAPS_LOCK', 'GLFW_KEY_CAPS_LOCK'),
  (0x3B, 'F1',                   'DOM_PK_F1', 'GLFW_KEY_F1'),
  (0x3C, 'F2',                   'DOM_PK_F2', 'GLFW_KEY_F2'),
  (0x3D, 'F3',                   'DOM_PK_F3', 'GLFW_KEY_F3'),
  (0x3E, 'F4',                   'DOM_PK_F4', 'GLFW_KEY_F4'),
  (0x3F, 'F5',                   'DOM_PK_F5', 'GLFW_KEY_F5'),
  (0x40, 'F6',                   'DOM_PK_F6', 'GLFW_KEY_F6'),
  (0x41, 'F7',                   'DOM_PK_F7', 'GLFW_KEY_F7'),
  (0x42, 'F8',                   'DOM_PK_F8', 'GLFW_KEY_F8'),
  (0x43, 'F9',                   'DOM_PK_F9', 'GLFW_KEY_F9'),
  (0x44, 'F10',                  'DOM_PK_F10', 'GLFW_KEY_F10'),
  (0x45, 'Pause',                'DOM_PK_PAUSE', 'GLFW_KEY_PAUSE'),
  (0x46, 'ScrollLock',           'DOM_PK_SCROLL_LOCK', 'GLFW_KEY_SCROLL_LOCK'),
  (0x47, 'Numpad7',              'DOM_PK_NUMPAD_7', 'GLFW_KEY_KP_7'),
  (0x48, 'Numpad8',              'DOM_PK_NUMPAD_8', 'GLFW_KEY_KP_8'),
  (0x49, 'Numpad9',              'DOM_PK_NUMPAD_9', 'GLFW_KEY_KP_9'),
  (0x4A, 'NumpadSubtract',       'DOM_PK_NUMPAD_SUBTRACT', 'GLFW_KEY_KP_SUBTRACT'),
  (0x4B, 'Numpad4',              'DOM_PK_NUMPAD_4', 'GLFW_KEY_KP_4'),
  (0x4C, 'Numpad5',              'DOM_PK_NUMPAD_5', 'GLFW_KEY_KP_5'),
  (0x4D, 'Numpad6',              'DOM_PK_NUMPAD_6', 'GLFW_KEY_KP_6'),
  (0x4E, 'NumpadAdd',            'DOM_PK_NUMPAD_ADD', 'GLFW_KEY_KP_ADD'),
  (0x4F, 'Numpad1',              'DOM_PK_NUMPAD_1', 'GLFW_KEY_KP_1'),
  (0x50, 'Numpad2',              'DOM_PK_NUMPAD_2', 'GLFW_KEY_KP_2'),
  (0x51, 'Numpad3',              'DOM_PK_NUMPAD_3', 'GLFW_KEY_KP_3'),
  (0x52, 'Numpad0',              'DOM_PK_NUMPAD_0', 'GLFW_KEY_KP_0'),
  (0x53, 'NumpadDecimal',        'DOM_PK_NUMPAD_DECIMAL', 'GLFW_KEY_KP_DECIMAL'),
  (0x54, 'PrintScreen',          'DOM_PK_PRINT_SCREEN', 'GLFW_KEY_PRINT_SCREEN'),
  # 0x0055 'Unidentified', ''
  (0x56, 'IntlBackslash',        'DOM_PK_INTL_BACKSLASH'),
  (0x57, 'F11',                  'DOM_PK_F11', 'GLFW_KEY_F11'),
  (0x58, 'F12',                  'DOM_PK_F12', 'GLFW_KEY_F12'),
  (0x59, 'NumpadEqual',          'DOM_PK_NUMPAD_EQUAL', 'GLFW_KEY_KP_EQUAL'),
  # 0x005A 'Unidentified', ''
  # 0x005B 'Unidentified', ''
  # 0x005C 'Unidentified', ''
  # 0x005D 'Unidentified', ''
  # 0x005E 'Unidentified', ''
  # 0x005F 'Unidentified', ''
  # 0x0060 'Unidentified', ''
  # 0x0061 'Unidentified', ''
  # 0x0062 'Unidentified', ''
  # 0x0063 'Unidentified', ''
  (0x64, 'F13',                  'DOM_PK_F13', 'GLFW_KEY_F13'),
  (0x65, 'F14',                  'DOM_PK_F14', 'GLFW_KEY_F14'),
  (0x66, 'F15',                  'DOM_PK_F15', 'GLFW_KEY_F15'),
  (0x67, 'F16',                  'DOM_PK_F16', 'GLFW_KEY_F16'),
  (0x68, 'F17',                  'DOM_PK_F17', 'GLFW_KEY_F17'),
  (0x69, 'F18',                  'DOM_PK_F18', 'GLFW_KEY_F18'),
  (0x6A, 'F19',                  'DOM_PK_F19', 'GLFW_KEY_F19'),
  (0x6B, 'F20',                  'DOM_PK_F20', 'GLFW_KEY_F20'),
  (0x6C, 'F21',                  'DOM_PK_F21', 'GLFW_KEY_F21'),
  (0x6D, 'F22',                  'DOM_PK_F22', 'GLFW_KEY_F22'),
  (0x6E, 'F23',                  'DOM_PK_F23', 'GLFW_KEY_F23'),
  # 0x006F 'Unidentified', ''
  (0x70, 'KanaMode',             'DOM_PK_KANA_MODE'),
  (0x71, 'Lang2',                'DOM_PK_LANG_2'),
  (0x72, 'Lang1',                'DOM_PK_LANG_1'),
  (0x73, 'IntlRo',               'DOM_PK_INTL_RO'),
  # 0x0074 'Unidentified', ''
  # 0x0075 'Unidentified', ''
  (0x76, 'F24',                  'DOM_PK_F24', 'GLFW_KEY_F24'),
  # 0x0077 'Unidentified', ''
  # 0x0078 'Unidentified', ''
  (0x79, 'Convert',              'DOM_PK_CONVERT'),
  # 0x007A 'Unidentified', ''
  (0x7B, 'NonConvert',           'DOM_PK_NON_CONVERT'),
  # 0x007C 'Unidentified', ''
  (0x7D, 'IntlYen',              'DOM_PK_INTL_YEN'),
  (0x7E, 'NumpadComma',          'DOM_PK_NUMPAD_COMMA'),
  # 0x007F 'Unidentified', ''
  (0xE00A, 'Paste',              'DOM_PK_PASTE'),
  (0xE010, 'MediaTrackPrevious', 'DOM_PK_MEDIA_TRACK_PREVIOUS'),
  (0xE017, 'Cut',                'DOM_PK_CUT'),
  (0xE018, 'Copy',               'DOM_PK_COPY'),
  (0xE019, 'MediaTrackNext',     'DOM_PK_MEDIA_TRACK_NEXT'),
  (0xE01C, 'NumpadEnter',        'DOM_PK_NUMPAD_ENTER', 'GLFW_KEY_KP_ENTER'),
  (0xE01D, 'ControlRight',       'DOM_PK_CONTROL_RIGHT', 'GLFW_KEY_RIGHT_CONTROL'),
  (0xE020, 'AudioVolumeMute',    'DOM_PK_AUDIO_VOLUME_MUTE'),
  (0xE020, 'VolumeMute',         'DOM_PK_AUDIO_VOLUME_MUTE'),
  (0xE021, 'LaunchApp2',         'DOM_PK_LAUNCH_APP_2'),
  (0xE022, 'MediaPlayPause',     'DOM_PK_MEDIA_PLAY_PAUSE'),
  (0xE024, 'MediaStop',          'DOM_PK_MEDIA_STOP'),
  (0xE02C, 'Eject',              'DOM_PK_EJECT'),
  (0xE02E, 'AudioVolumeDown',    'DOM_PK_AUDIO_VOLUME_DOWN'),
  (0xE02E, 'VolumeDown',         'DOM_PK_AUDIO_VOLUME_DOWN'),
  (0xE030, 'AudioVolumeUp',      'DOM_PK_AUDIO_VOLUME_UP'),
  (0xE030, 'VolumeUp',           'DOM_PK_AUDIO_VOLUME_UP'),
  (0xE032, 'BrowserHome',        'DOM_PK_BROWSER_HOME'),
  (0xE035, 'NumpadDivide',       'DOM_PK_NUMPAD_DIVIDE', 'GLFW_KEY_KP_DIVIDE'),
  #  (0xE037, 'PrintScreen',        'DOM_PK_PRINT_SCREEN'),
  (0xE038, 'AltRight',           'DOM_PK_ALT_RIGHT', 'GLFW_KEY_RIGHT_ALT'),
  (0xE03B, 'Help',               'DOM_PK_HELP'),
  (0xE045, 'NumLock',            'DOM_PK_NUM_LOCK', 'GLFW_KEY_NUM_LOCK'),
  #  (0xE046, 'Pause', 'DOM_PK_'), # Says Ctrl+Pause
  (0xE047, 'Home',               'DOM_PK_HOME', 'GLFW_KEY_HOME'),
  (0xE048, 'ArrowUp',            'DOM_PK_ARROW_UP', 'GLFW_KEY_UP'),
  (0xE049, 'PageUp',             'DOM_PK_PAGE_UP', 'GLFW_KEY_PAGE_UP'),
  (0xE04B, 'ArrowLeft',          'DOM_PK_ARROW_LEFT', 'GLFW_KEY_LEFT'),
  (0xE04D, 'ArrowRight',         'DOM_PK_ARROW_RIGHT', 'GLFW_KEY_RIGHT'),
  (0xE04F, 'End',                'DOM_PK_END', 'GLFW_KEY_END'),
  (0xE050, 'ArrowDown',          'DOM_PK_ARROW_DOWN', 'GLFW_KEY_DOWN'),
  (0xE051, 'PageDown',           'DOM_PK_PAGE_DOWN', 'GLFW_KEY_PAGE_DOWN'),
  (0xE052, 'Insert',             'DOM_PK_INSERT', 'GLFW_KEY_INSERT'),
  (0xE053, 'Delete',             'DOM_PK_DELETE', 'GLFW_KEY_DELETE'),
  (0xE05B, 'MetaLeft',           'DOM_PK_META_LEFT', 'GLFW_KEY_LEFT_SUPER'),
  (0xE05B, 'OSLeft',             'DOM_PK_OS_LEFT'),
  (0xE05C, 'MetaRight',          'DOM_PK_META_RIGHT', 'GLFW_KEY_RIGHT_SUPER'),
  (0xE05C, 'OSRight',            'DOM_PK_OS_RIGHT'),
  (0xE05D, 'ContextMenu',        'DOM_PK_CONTEXT_MENU', 'GLFW_KEY_MENU'),
  (0xE05E, 'Power',              'DOM_PK_POWER'),
  (0xE065, 'BrowserSearch',      'DOM_PK_BROWSER_SEARCH'),
  (0xE066, 'BrowserFavorites',   'DOM_PK_BROWSER_FAVORITES'),
  (0xE067, 'BrowserRefresh',     'DOM_PK_BROWSER_REFRESH'),
  (0xE068, 'BrowserStop',        'DOM_PK_BROWSER_STOP'),
  (0xE069, 'BrowserForward',     'DOM_PK_BROWSER_FORWARD'),
  (0xE06A, 'BrowserBack',        'DOM_PK_BROWSER_BACK'),
  (0xE06B, 'LaunchApp1',         'DOM_PK_LAUNCH_APP_1'),
  (0xE06C, 'LaunchMail',         'DOM_PK_LAUNCH_MAIL'),
  (0xE06D, 'LaunchMediaPlayer',  'DOM_PK_LAUNCH_MEDIA_PLAYER'),
  (0xE06D, 'MediaSelect',        'DOM_PK_MEDIA_SELECT')
  #  (0xE0F1, 'Lang2', 'DOM_PK_'), Hanja key
  #  (0xE0F2, 'Lang2', 'DOM_PK_'), Han/Yeong
]


def hash(s, k1, k2):
  h = 0
  for c in s:
    h = int(int(int(h ^ k1) << k2) ^ ord(c)) & 0xFFFFFFFF
  return h


def hash_all(k1, k2):
  hashes = {}
  str_to_hash = {}
  for s in input_strings:
    h = hash(s[1], k1, k2)
    print('String "' + s[1] + '" hashes to %s ' % hex(h), file=sys.stderr)
    if h in hashes:
      print('Collision! Earlier string ' + hashes[h] + ' also hashed to %s!' % hex(h), file=sys.stderr)
      return None
    else:
      hashes[h] = s[1]
      str_to_hash[s[1]] = h
  return (hashes, str_to_hash)


# Find an approprite hash function that is collision free within the set of all input strings
# Try hash function format h_i = ((h_(i-1) ^ k_1) << k_2) ^ s_i, where h_i is the hash function
# value at step i, k_1 and k_2 are the constants we are searching, and s_i is the i'th input
# character
perfect_hash_table = None

# Last used perfect hash constants.  Stored here so that this script will
# produce the same output it did when the current output was generated.
k1 = 0x7E057D79
k2 = 3
perfect_hash_table = hash_all(k1, k2)

while not perfect_hash_table:
  # The search space is super-narrow, but since there are so few items to hash, practically
  # almost any choice gives a collision free hash.
  k1 = int(random.randint(0, 0x7FFFFFFF))
  k2 = int(random.uniform(1, 8))
  perfect_hash_table = hash_all(k1, k2)

hash_to_str, str_to_hash = perfect_hash_table

print('Found collision-free hash function!', file=sys.stderr)
print('h_i = ((h_(i-1) ^ %s) << %s) ^ s_i' % (hex(k1), hex(k2)), file=sys.stderr)


def pad_to_length(s, length):
  return s + max(0, length - len(s)) * ' '


longest_dom_pk_code_length = max(map(len, [x[2] for x in input_strings]))
longest_key_code_length = max(map(len, [x[1] for x in input_strings]))
longest_glfw_code_length = max(map(len, [x[3] for x in input_strings if len(x) > 3]))


mapping_file = open('src/cpp/emscripten/glfw3/KeyboardMapping.h', 'w')

# Generate the output file:

mapping_file.write('''\
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

''')

duplicates = set()

for s in input_strings:
    comment = ''
    if s[2] in duplicates:
        comment = '// '
    else:
        duplicates.add(s[2])
    mapping_file.write(comment + 'constexpr glfw_scancode_t ' + pad_to_length(s[2], longest_dom_pk_code_length) + ' = 0x%04X; /* "%s */' % (s[0], pad_to_length(s[1] + '"', longest_key_code_length + 1)) + '\n')

# keyboardEventCodeToScancode
mapping_file.write('''
//------------------------------------------------------------------------
// keyboardEventCodeToScancode: maps the code coming from keyboardEvent.code 
// (which is a string!) to a unique scancode number 
//------------------------------------------------------------------------
constexpr glfw_scancode_t keyboardEventCodeToScancode(char const *iCode)
{
  if (!iCode) return 0;

  /* Compute the collision free hash. */
  unsigned int hash = 0;
  while(*iCode) hash = ((hash ^ 0x%04XU) << %d) ^ (unsigned int)*iCode++;

  switch(hash)
  {
''' % (k1, k2))

for s in input_strings:
    mapping_file.write('    case 0x%08XU /* %s */: return %s /* 0x%04X */' % (str_to_hash[s[1]], pad_to_length(s[1], longest_key_code_length), pad_to_length(s[2] + ';', longest_dom_pk_code_length + 1), s[0]) + '\n')

mapping_file.write('''    default: return DOM_PK_UNKNOWN;
  }
}

''')

# scancodeToString
mapping_file.write('''
//------------------------------------------------------------------------
// scancodeToString: maps glfw_scancode_t to string
// returns nullptr when no mapping
//------------------------------------------------------------------------
constexpr char const *scancodeToString(glfw_scancode_t iCode)
{
  switch(iCode)
  {
''')

duplicates = set()
for s in input_strings:
    if s[0] not in duplicates:
        mapping_file.write('    case %s return "%s";' % (pad_to_length(s[2] + ':', longest_dom_pk_code_length + 1), s[2]) + '\n')
        duplicates.add(s[0])

mapping_file.write('''    default: return nullptr;
  }
}

''')

# scancodeToKeyCode
mapping_file.write('''
//------------------------------------------------------------------------
// scancodeToKeyCode: maps glfw_scancode_t to the glfw key code
// returns GLFW_KEY_UNKNOWN when not match
//------------------------------------------------------------------------
constexpr glfw_key_t scancodeToKeyCode(glfw_scancode_t iCode)
{
  switch(iCode)
  {
''')

for s in input_strings:
    if len(s) > 3:
        mapping_file.write('    case %s return %s;' % (pad_to_length(s[2] + ':', longest_dom_pk_code_length + 1), s[3]) + '\n')

mapping_file.write('''    default: return GLFW_KEY_UNKNOWN;
  }
}

''')

# keyCodeToScancode
mapping_file.write('''
//------------------------------------------------------------------------
// keyCodeToScancode: maps glfw key code to glfw_scancode_t
// returns DOM_PK_UNKNOWN when not match
//------------------------------------------------------------------------
constexpr glfw_scancode_t keyCodeToScancode(glfw_key_t iCode)
{
  switch(iCode)
  {
''')

for s in input_strings:
    if len(s) > 3:
        mapping_file.write('    case %s return %s;' % (pad_to_length(s[3] + ':', longest_glfw_code_length + 1), s[2]) + '\n')

mapping_file.write('''    default: return DOM_PK_UNKNOWN;
  }
}

''')

mapping_file.write('''
} // namespace keyboard
} // namespace emscripten::glfw3

#endif // EMSCRIPTEN_GLFW_KEYBOARD_MAPPING_H
''')

mapping_file.close()

# possible javascript implementation if necessary
# mapping_file = open('src/js/lib_emscripten_glfw3_events.js', 'w')
#
# mapping_file.write('''
# let impl = {
#   keyboardEventCodeToScancode: (code) => {
#     if (!code) return 0;
#
#     let hash = 0;
#     for(let i = 0; i < code.length; i++) {
#       hash = ((hash ^ 0x%04X) << %d) ^ code.charCodeAt(i);
#     }
#
#     switch(hash)
#     {
# ''' % (k1, k2))
#
# for s in input_strings:
#     mapping_file.write('      case 0x%08X: return 0x%04X;' % (str_to_hash[s[1]], s[0]) + '\n')
#
# mapping_file.write('''      default: return -1;
#     } // switch
#   } // keyboardEventCodeToScancode
# ''')
#
# mapping_file.write('''
# } // impl
#
# mergeInto(LibraryManager.library, impl);
#
# ''')
#
# mapping_file.close()
