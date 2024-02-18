# Copyright 2024 The Emscripten Authors.  All rights reserved.
# Emscripten is available under two separate licenses, the MIT license and the
# University of Illinois/NCSA Open Source License.  Both these licenses can be
# found in the LICENSE file.

import os
from typing import Dict

TAG = '1.0.5'
HASH = '4ef49ad23bd70a03639df4c8b0fdfe5b8c25bfe4d183c52a7e2d6e7285ab23c09b688b898911f0b9ce0fd014840a73c220592c01bf6e3af5280d927a2718f8aa'

# contrib port information (required)
URL = 'https://github.com/pongasoft/emscripten-glfw'
DESCRIPTION = 'This project is an emscripten port of GLFW written in C++ for the web/webassembly platform'
LICENSE = 'Apache 2.0 license'

OPTIONS = {
  'disableWarning': 'Boolean to disable warnings emitted by the library',
  'disableJoystick': 'Boolean to disable support for joystick entirely',
  'disableMultiWindow': 'Boolean to disable multi window support which makes the code smaller and faster'
}

# user options (from --use-port)
opts: Dict[str, bool] = {
  'disableWarning': False,
  'disableJoystick': False,
  'disableMultiWindow': False
}


def get_lib_name(settings):
  return (f'lib_{name}_{TAG}' +
          ('-nw' if opts['disableWarning'] else '') +
          ('-nj' if opts['disableJoystick'] else '') +
          ('-sw' if opts['disableMultiWindow'] else '') +
          '.a')


def get(ports, settings, shared):
  # get the port
  ports.fetch_project(name,
                      f'https://github.com/pongasoft/emscripten-glfw/releases/download/v{TAG}/emscripten-glfw3-{TAG}.zip',
                      sha512hash=HASH)

  def create(final):
    root_path = os.path.join(ports.get_dir(), name)
    source_path = os.path.join(root_path, 'src', 'cpp')
    source_include_paths = [os.path.join(root_path, 'external', 'GLFW'), os.path.join(root_path, 'include', 'GLFW')]
    for source_include_path in source_include_paths:
      ports.install_headers(source_include_path, target='GLFW')

    flags = []

    if opts['disableWarning']:
      flags += ['-DEMSCRIPTEN_GLFW3_DISABLE_WARNING']

    if opts['disableJoystick']:
      flags += ['-DEMSCRIPTEN_GLFW3_DISABLE_JOYSTICK']

    if opts['disableMultiWindow']:
      flags += ['-DEMSCRIPTEN_GLFW3_DISABLE_MULTI_WINDOW_SUPPORT']

    ports.build_port(source_path, final, name, includes=source_include_paths, flags=flags)

  lib = shared.cache.get_lib(get_lib_name(settings), create, what='port')
  if os.path.getmtime(lib) < os.path.getmtime(__file__):
      clear(ports, settings, shared)
      lib = shared.cache.get_lib(get_lib_name(settings), create, what='port')
  return [lib]

def clear(ports, settings, shared):
  shared.cache.erase_lib(get_lib_name(settings))


def linker_setup(ports, settings):
  root_path = os.path.join(ports.get_dir(), name)
  source_js_path = os.path.join(root_path, 'src', 'js', 'lib_emscripten_glfw3.js')
  settings.JS_LIBRARIES += [source_js_path]


# Using contrib.glfw3 to avoid installing headers into top level include path
# so that we don't conflict with the builtin GLFW headers that emscripten
# includes
def process_args(ports):
  return ['-isystem', ports.get_include_dir(name)]


def handle_options(options, error_handler):
  for option, value in options.items():
    if value.lower() in {'true', 'false'}:
      opts[option] = value.lower() == 'true'
    else:
      error_handler(f'{option} is expecting a boolean, got {value}')
