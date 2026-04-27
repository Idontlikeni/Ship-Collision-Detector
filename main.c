#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define Vec3To2(v) ((Vector2) { v.x, v.y })

Vector2 T1;
Vector2 T2;

typedef struct PointsArray
{
    int n;
    Vector2 items[100];
} PointsArray;

typedef struct Vec3Pair
{
    Vector3 first;
    Vector3 second;
} Vec3Pair;

Vec3Pair CalculateIntersection(Vector2 targetPos, float radius, float angle)
{
    // (x - x1)^2 + (y - y1)^2 == r^2
    // y = sin(a) * t, x = cos(a) * t
    // (sin*t - x1)^2 + (cos*t - y1)^2 == r^2

    float cos_a = cos(angle), sin_a = sin(angle);
    float a = 1.0f;
    float b = targetPos.x * cos_a + targetPos.y * sin_a;
    float c = targetPos.x * targetPos.x + targetPos.y * targetPos.y - radius * radius;

    float D = b*b - 4*a*c;
    float t1 = (-b + sqrt(D))/(2*a*c);
    float t2 = (-b - sqrt(D))/(2*a*c);

    return (Vec3Pair){ {t1*cos_a, t1*sin_a, t1}, {t2*cos_a, t2*sin_a, t2} };
}

Vector2* CalculateCollisionArea(Vector2 targetPos, Vector2 targetVec, float targetRadius, float shipSpeed, int pointsCount)
{
    // float distance = sqrtf(targetX * targetX + targetY * targetY);
    float d = Vector2Length(targetPos); // d - distance to the target
    // printf("Distance: %02f \n", d);
    float m = sqrtf(d * d - targetRadius * targetRadius); // Length of the tangent line (касательной)
    
    float h = targetPos.x, k = targetPos.y; // (h, k) - pos of target
    float r = targetRadius; // - radius of target
    Vector2 F = {m*m*h/(d*d), m*m*k/(d*d)}; // F - Точка пересечения поляры и вектора от нашего корабля к центру окружнсти
    Vector2 s = {-(k*m*r/(d*d)), (h*m*r/(d*d))}; // s - Вектор поляры
    
    Vector2* collisionPoints = calloc(pointsCount, sizeof(Vector2));
    if(collisionPoints == NULL)
    {
        printf("Unable to create collision points array");
        return NULL;
    }

    /* for(int i = 0; i < pointsCount; ++i)
    {
        collisionPoints[i] = CalculateIntersection(targetPos, targetRadius, ...);
    } */

    // PointsArray points;
    T1 = Vector2Add(F, s);
    T2 = Vector2Add(F, Vector2Negate(s));
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib example - basic window");

    int x = 100, y = 100;
    float r = 50.0f;
    Vector2 shipScreenPos = { WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f };
    Vector2 mousePos;
    while (!WindowShouldClose())
    {
        // mousePos = Vector2Add(GetMousePosition(), StartPos);
        mousePos = GetMousePosition();
        r += GetMouseWheelMove();

        // Calculating a Decart positiong of target
        Vector2 relativeTargetPosition = {mousePos.x - shipScreenPos.x, -(mousePos.y - shipScreenPos.y)};
        float targetAngle = atan2(relativeTargetPosition.y, relativeTargetPosition.x);

        CalculateCollisionArea(relativeTargetPosition, (Vector2){0, 0}, r, 0.0f, 10);
        Vec3Pair intersectionPoints = CalculateIntersection(relativeTargetPosition, r, targetAngle);
        // Calculating the positiong of resulting area on the screen
        // areaToScreen();
        Vector2 T1Screen = (Vector2){shipScreenPos.x + T1.x, shipScreenPos.y - T1.y};
        Vector2 T2Screen = (Vector2){shipScreenPos.x + T2.x, shipScreenPos.y - T2.y};

        printf("Angle: %.2f \n", targetAngle);
        BeginDrawing();
            DrawText(TextFormat("targetPos: %.2f %.2f", relativeTargetPosition.x, relativeTargetPosition.y), 10, 40, 20, LIGHTGRAY);
            ClearBackground(RAYWHITE);
            DrawCircleV(mousePos, r, RED); 
            DrawLineV(shipScreenPos, T1Screen, BLACK);
            DrawLineV(shipScreenPos, T2Screen, BLACK);
            DrawLineV(shipScreenPos, Vec3To2(intersectionPoints.first), BLACK);
            DrawLineV(shipScreenPos, Vec3To2(intersectionPoints.second), BLACK);
            // DrawLine(StartPos.x, 0, StartPos.x, WINDOW_HEIGHT, LIGHTGRAY);
            // DrawLine(0, StartPos.y, WINDOW_WIDTH, StartPos.y, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
