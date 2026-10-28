# Copyright (c) 2025 pongasoft
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# @author Yan Pujante

TAG = '3.5.1.20261028'

EXTERNAL_PORT = f'https://github.com/pongasoft/emscripten-glfw/releases/download/v{TAG}/emscripten-glfw3-{TAG}.zip'
SHA512 = '4acc1aa275cc90dea906bc2fe3e41e5d21ea2a2b50e61a6ae1330c0e48189ad1000c9cf530958a6de99c5c3454783f8638a3024f9ef7c17fa254ed49a4ee01e4'
PORT_FILE = 'port/glfw3.py'

# contrib port information (required)
URL = 'https://github.com/pongasoft/emscripten-glfw'
DESCRIPTION = 'This project is an emscripten port of GLFW 3.5 written in C++ for the web/webassembly platform'
LICENSE = 'Apache 2.0 license'
