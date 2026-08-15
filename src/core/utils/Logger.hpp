#pragma once

#if !defined(__EMSCRIPTEN__)
    #include <spdlog/spdlog.h>
    
    // Native Environment: Directly maps to our ultra-low latency spdlog async engine.
    // Full zero-allocation background threading with fmt {} native support.
    #define LOG_INFO(...)  spdlog::info(__VA_ARGS__)
    #define LOG_WARN(...)  spdlog::warn(__VA_ARGS__)
    #define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#else
    #include <iostream>
    
    namespace enma::core::utils {
        // WebAssembly Environment: Emscripten cannot natively use spdlog's async threaded queue.
        // We provide a universal fallback that parses the exact same `{}` fmt syntax dynamically
        // and pipes it to std::cout (which binds directly to the browser's console.log).
        
        template <typename T>
        inline void LogWasmPrint(std::ostream& os_, const T& arg_) {
            os_ << arg_;
        }

        template <typename... Args>
        inline void LogWasm(const char* level_, const char* format_, Args&&... args_) {
            std::cout << "[" << level_ << "] ";
            
            auto formatStr = format_;
            auto printArg = [&](const auto& arg) {
                while (*formatStr) {
                    if (*formatStr == '{' && *(formatStr + 1) == '}') {
                        LogWasmPrint(std::cout, arg);
                        formatStr += 2;
                        return;
                    }
                    std::cout << *formatStr++;
                }
            };
            
            // Expand the variadic template to replace each {} with the corresponding argument
            (printArg(args_), ...);
            
            // Print any remaining characters in the format string
            while (*formatStr) {
                std::cout << *formatStr++;
            }
            std::cout << "\n";
        }
        
        inline void LogWasm(const char* level_, const char* format_) {
            std::cout << "[" << level_ << "] " << format_ << "\n";
        }
    }

    #define LOG_INFO(...)  enma::core::utils::LogWasm("info", __VA_ARGS__)
    #define LOG_WARN(...)  enma::core::utils::LogWasm("warn", __VA_ARGS__)
    #define LOG_ERROR(...) enma::core::utils::LogWasm("error", __VA_ARGS__)
#endif
