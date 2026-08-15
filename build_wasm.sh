#!/bin/bash

BUILD_DIR="build_wasm"
mkdir -p "$BUILD_DIR"

# Ensure emsdk is available, if not install it locally in the build folder
if ! command -v emcmake &> /dev/null; then
    echo "emcmake not found. Setting up Emscripten SDK..."
    if [ ! -d "$BUILD_DIR/emsdk" ]; then
        git clone https://github.com/emscripten-core/emsdk.git "$BUILD_DIR/emsdk"
    fi
    cd "$BUILD_DIR/emsdk"
    ./emsdk install latest
    ./emsdk activate latest
    cd ../..
    source "$BUILD_DIR/emsdk/emsdk_env.sh"
fi

cd "$BUILD_DIR"
emcmake cmake ..
cmake --build .
