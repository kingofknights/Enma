#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include "core/utils/Logger.hpp"

#if !defined(PLATFORM_WEB)
    #include <spdlog/async.h>
    #include <spdlog/sinks/stdout_color_sinks.h>
#endif

const int screenWidth = 800;
const int screenHeight = 450;

void UpdateDrawFrame(void);

int main(void)
{
#if !defined(PLATFORM_WEB)
    // Initialize spdlog asynchronous thread pool (8192 queue size, 1 backing thread)
    // This perfectly abides by our zero-latency hot-path rule!
    spdlog::init_thread_pool(8192, 1);
    auto async_logger = spdlog::stdout_color_mt<spdlog::async_factory>("async_logger");
    spdlog::set_default_logger(async_logger);
#endif

    // Universally logs via spdlog async on Native and std::cout parsing on WebAssembly!
    LOG_INFO("Enma Trading Engine starting... {} cores active, {} environment", 12, "Production");

    InitWindow(screenWidth, screenHeight, "Enma - Trading Dashboard");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();

    return 0;
}

void UpdateDrawFrame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! Raylib is running!", 220, 200, 20, LIGHTGRAY);
    EndDrawing();
}
