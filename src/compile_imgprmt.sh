#!/bin/bash

# compile_imgprmt.sh

g++ main.cpp fileChecks.cpp programArgs.cpp convertString.cpp replaceChars.cpp information.cpp segmentsVec.cpp valueUpdater.cpp eraseSegments.cpp imgPrmt.cpp transcodeImage.cpp -Wall -O2 -lturbojpeg -s -o imgprmt

if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable 'imgprmt' created."
else
    echo "Compilation failed."
    exit 1
fi
