# Project Guidelines: RainApp

## 1. Core Identity
This project is an **ultra-low latency trading application**. Every architectural and coding decision must prioritize absolute minimal latency, deterministic execution time, and extreme performance.

## 2. Language & Standards
- **C++20**: All code must be written using the C++20 standard.
- Prefer compile-time evaluation (`constexpr`, `consteval`) to shift computation overhead from runtime to compile time.
- Leverage C++20 features (Concepts, Ranges, Coroutines if applicable) carefully, ensuring they do not introduce hidden runtime costs.

## 3. Ultra-Low Latency Engineering Rules
- **No Allocations on the Hot Path**: Zero dynamic memory allocation (`new`, `malloc`, `std::string`, `std::vector` resizing) is permitted during the critical path (e.g., processing market data, sending orders).
- **Pre-allocation**: Use pre-allocated object pools, ring buffers, `std::array`, and fixed-size buffers initialized during application startup.
- **Concurrency & Locking**: 
  - Strictly **NO locks** (`std::mutex`, spinlocks) in the hot path.
  - Inter-thread communication must be achieved using lock-free data structures (e.g., Single-Producer Single-Consumer queues) and `std::atomic` operations with memory order optimization.
- **Cache Optimization**: 
  - Maximize CPU cache hits. Design data structures for spatial and temporal locality.
  - Pad structures to avoid false sharing between threads on different cores.
- **Branch Prediction**: Avoid complex branching in the hot path. Use C++20 `[[likely]]` and `[[unlikely]]` attributes to assist compiler optimization and CPU branch predictors.
- **System Calls**: Absolutely no system calls (I/O, standard logging, file writing, blocking network calls) in the critical path. Asynchronous logging must be dispatched to a background thread.
- **Thread Affinity**: Critical trading threads should be designed to be pinned to specific isolated CPU cores.

## 4. Visualization & GUI (Raylib)
- The visualization layer (built with Raylib) must be completely decoupled from the trading engine.
- The UI thread must never block, lock, or slow down the trading/hot-path threads. Use lock-free queues or double-buffering to read state into the GUI.

## 5. Build & Tooling
- **Cross-Platform**: All code must be carefully written to compile and run seamlessly on **Win32**, **Linux**, and **WebAssembly (Browser)**. Platform-specific features or APIs must be guarded with correct preprocessor macros.
- The build system uses CMake.
- Release builds must compile with aggressive optimization flags (e.g., `-O3`, `-march=native`, `-flto`).

## 6. Coding Style
- **Naming Conventions**: 
  - All file names must be `PascalCase` and use `.hpp` and `.cpp` extensions (e.g., `MarketData.hpp`).
  - All function names must be `PascalCase` (e.g., `ProcessMarketData()`).
  - Member variables in both classes and structs must be prefixed with `_` (e.g., `_price`, `_orderId`).
  - All function parameters must have the suffix `_` (e.g., `price_`, `orderId_`).
- **Strong Typing / Type Aliases**: 
  - Declare strict data types in advance for domain concepts to improve readability and enforce type safety. 
  - Use `using` directives (e.g., `using PriceT = int32_t;`, `using QuantityT = uint32_t;`).
- **Containers**: 
  - Any use of a container must be explicitly declared and instantiated first (e.g., upfront during application initialization). No container should be dynamically created or resized during the hot path.
- **Views & Spans**:
  - Always use `std::span` instead of raw pointers with sizes when passing arrays or memory buffers.
  - Always use `std::string_view` instead of `const char*` or `const std::string&` when passing read-only strings.
- **Fixed-width Integer Types**:
  - Never use loosely sized types like `int`, `long`, or `short`.
  - Always use explicitly sized types from `<cstdint>` (e.g., `uint32_t`, `int64_t`) to ensure cross-platform deterministic sizing.
- **Functions & Signatures**:
  - Always use **trailing return types** (e.g., `auto CalculatePrice() -> uint32_t`).
  - Always use `[[nodiscard]]` for functions that return a value.
- **Class/Struct Layout**:
  - The strict layout order for classes must be:
    1. Container declarations (if any)
    2. `public` functions and members
    3. `protected` functions and members
    4. `private` functions and members
  - When declaring `private` state, group it into a nested `struct` if possible (e.g., `struct PrivateData`) to improve encapsulation and organization.
  - **Cache-Friendly Data Layout**: Always order members by size (largest to smallest) or usage frequency to minimize padding and maximize cache locality. Use explicit padding where required to prevent false sharing.

## 7. Architecture & Code Quality
- **Design Patterns & SOLID**: The codebase must adhere to strictly defined architecture following **SOLID principles** and appropriate **Design Patterns** (e.g., Strategy, Factory, Observer) wherever they do not compromise the zero-allocation/low-latency constraints.
- **Continuous Refactoring**: If a block of code can be refactored into a smaller, more concise function, or if it can be optimized for better performance or readability, **always refactor and optimize it immediately**.
