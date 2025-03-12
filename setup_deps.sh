#!/bin/bash

git clone https://github.com/juce-framework/JUCE.git # whatever version was in early 2025 :)
cd JUCE
# cmake version 3.27.7
cmake . -B cmake-build -DJUCE_BUILD_EXAMPLES=ON -DJUCE_BUILD_EXTRAS=ON
cmake --build cmake-build --target CMake