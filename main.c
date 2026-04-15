#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


Vector2 T1;
Vector2 T2;

Vector2 CalculateCollisionArea(Vector2 targetPos, Vector2 targetVec, float targetRadius, float shipSpeed){
    // float distance = sqrtf(targetX * targetX + targetY * targetY);
    float d = Vector2Length(targetPos); // d - distance to the target
    printf("Distance: %02f \n", d);
    float m = sqrtf(d * d - targetRadius * targetRadius); // Length of the tangent line
    
    float h = targetPos.x, k = targetPos.y;
    float r = targetRadius;
    Vector2 F = {m*m*h/(d*d), m*m*k/(d*d)}; // F - Точка пересечения поляры и вектора от нашего корабля к центру окружнсти
    Vector2 s = {-(h*m*r/(d*d)), (k*m*r/(d*d))}; // s - Вектор поляры
    T1 = Vector2Add(F, s);
    T2 = Vector2Add(F, Vector2Negate(s));
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib example - basic window");

    int x = 100, y = 100;
    float r = 50.0f;
    Vector2 StartPos = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
    Vector2 SysTransform = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
    Vector2 null = {0.0f, 0.0f};
    Vector2 mousePos;
    while (!WindowShouldClose())
    {
        mousePos = Vector2Add(GetMousePosition(), StartPos);
        r += GetMouseWheelMove();
        CalculateCollisionArea(mousePos, null, r, 0.0f);
        BeginDrawing();
            DrawText(TextFormat("targetPos: %02f %02f", mousePos.x, mousePos.y), 10, 40, 20, LIGHTGRAY);
            ClearBackground(RAYWHITE);
            DrawCircleV(Vector2Subtract(mousePos, StartPos), r, RED); 
            DrawLineV(StartPos, T1, BLACK);
            DrawLineV(StartPos, T2, BLACK);
            // DrawLine(StartPos.x, 0, StartPos.x, WINDOW_HEIGHT, LIGHTGRAY);
            // DrawLine(0, StartPos.y, WINDOW_WIDTH, StartPos.y, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
