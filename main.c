#include "inc/raylib.h"

int main(void)
{
    InitWindow(800, 450, "Squara");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Raylib Hello World!", 150, 200, 40, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
