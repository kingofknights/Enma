# Enma

**Enma** is an ultra-low latency trading application designed with extreme performance and deterministic execution times in mind. Built entirely in modern C++20, it enforces strict zero-allocation constraints, lock-free concurrency, and maximum cache friendliness. The visualization and GUI layer is fully decoupled and powered by [Raylib](https://www.raylib.com/).

## 🚀 Key Features

- **C++20 Architecture**: Leverages strict compile-time computations, fixed-width integer types, and modern C++ semantics.
- **Ultra-Low Latency**: Follows strict zero-allocation in the hot-path and uses lock-free queues for inter-thread communication.
- **Cross-Platform**: Seamlessly compiles and runs on **Win32**, **Linux**, and **WebAssembly (Browser)**.
- **Raylib GUI**: Offloads UI visualization to an isolated thread entirely decoupled from the core trading engine.

## 🛠️ Building the Project

This project uses CMake and seamlessly manages its own dependencies via `FetchContent` (or pre-compiled downloads). 

### 1. Native Build (Linux & Windows)

Run the automated bash script, which will setup any missing headers and build the desktop application:

```bash
./build_native.sh
```

Execute the built native application:
```bash
./build_native/RainApp
```

### 2. WebAssembly Build (Browser)

The WebAssembly build script automatically downloads and locally isolates the [Emscripten SDK](https://emscripten.org/) inside the build directory to compile the project for the web.

```bash
./build_wasm.sh
```

To run the WebAssembly build locally, use the provided Python script to spin up a local server with the correct MIME types and HTTP Headers:
```bash
./serve_wasm.py
```
Then open `http://localhost:8080/RainApp.html` in your web browser.

## 📜 Development Guidelines

Enma adheres strictly to a set of robust architectural guidelines ensuring execution speed is never compromised. For complete details on the coding style, class structures, naming conventions, and constraints, please read the [AGENTS.md](AGENTS.md) file.
