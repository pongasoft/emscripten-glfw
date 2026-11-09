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

TAG = '3.5.1.20261109'

EXTERNAL_PORT = f'https://github.com/pongasoft/emscripten-glfw/releases/download/v{TAG}/emscripten-glfw3-{TAG}.zip'
SHA512 = 'e4b26b5d627476fb14ea900ebc3a4050c124e1f8fd5a600a887850ceed7d9ed8dd617ac16c7d1181839a26c553d8775093996d9c747e322d84a3236a9ca22791'
PORT_FILE = 'port/glfw3.py'

# contrib port information (required)
URL = 'https://github.com/pongasoft/emscripten-glfw'
DESCRIPTION = 'This project is an emscripten port of GLFW 3.5 written in C++ for the web/webassembly platform'
LICENSE = 'Apache 2.0 license'
