#!/usr/bin/env python3
import sys
import shutil
import os
import pathlib

# Retrieve arguments
binary_dir = sys.argv[1]
version = sys.argv[2]

local_port_file = os.path.join(binary_dir, 'emscripten-glfw3-local.py')
shutil.copy2('port/emscripten-glfw3.py', local_port_file)

sha512_file = os.path.join(binary_dir, 'archive', f'emscripten-glfw3-{version}.zip.sha512')

# Read sha512_file and extract the sum
with open(sha512_file, 'r') as file:
    sha512_sum = file.read().split(' ')[0]

# Open the file in read & write mode ('r+')
with open(local_port_file, 'r+') as file:
    lines = file.readlines()
    file.seek(0)
    for line in lines:
        if 'SHA512 = ' in line:
            file.write(line.replace(line, f"SHA512 = '{sha512_sum}'\n"))
        elif 'TAG = ' in line:
            file.write(line.replace(line, f"TAG = '{version}'\n"))
        elif 'EXTERNAL_PORT = ' in line:
            zip_file = os.path.join(binary_dir, 'archive', f'emscripten-glfw3-{version}.zip')
            zip_url = pathlib.Path(zip_file).as_uri()
            file.write(f'# {line}')
            file.write(line.replace(line, f"EXTERNAL_PORT = '{zip_url}'\n"))
        else:
            file.write(line)
    file.truncate()

print(f'Created local port {local_port_file}\n')
