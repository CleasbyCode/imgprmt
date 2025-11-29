#!/bin/bash

# compile_imgprmt.sh

g++ -std=c++23 -O3 -march=native -pipe -Wall -Wextra -Wpedantic -DNDEBUG -s -flto=auto -fuse-linker-plugin imgprmt.cpp -lturbojpeg -o imgprmt

if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable 'imgprmt' created."
else
    echo "Compilation failed."
    exit 1
fi
