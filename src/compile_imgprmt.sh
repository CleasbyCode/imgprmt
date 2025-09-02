#!/bin/bash

# compile_imgprmt.sh

g++ -std=c++20 main.cpp fileChecks.cpp programArgs.cpp information.cpp segmentsVec.cpp -Wall -O3 -lturbojpeg -s -o imgprmt

if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable 'imgprmt' created."
else
    echo "Compilation failed."
    exit 1
fi
