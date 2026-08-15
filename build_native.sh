#!/bin/bash

# If on Linux, try to ensure we have the necessary X11 headers for building raylib from source
if [ "$(uname)" == "Linux" ]; then
    if ! dpkg -s libxinerama-dev &> /dev/null; then
        echo "Missing X11 development headers required for Raylib."
        echo "Attempting to install them via apt-get (this may prompt for your sudo password)..."
        sudo apt-get update && sudo apt-get install -y libxinerama-dev libxcursor-dev libxrandr-dev libxi-dev libgl1-mesa-dev libx11-dev libxext-dev
    fi
fi

mkdir -p build_native
cd build_native
cmake ..
cmake --build .
