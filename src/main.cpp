#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

const int screenWidth = 800;
const int screenHeight = 450;

void UpdateDrawFrame(void);

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Raylib Web/Native App");

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
