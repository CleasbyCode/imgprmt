#!/bin/bash

# compile_imgprmt.sh

g++ -std=c++20 imgprmt.cpp -Wall -O3 -lturbojpeg -s -o imgprmt

if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable 'imgprmt' created."
else
    echo "Compilation failed."
    exit 1
fi
