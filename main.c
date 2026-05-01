#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

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
    float cos_a = cos(angle), sin_a = sin(angle);
    float a = 1.0f;
    float b = -2.0f * (targetPos.x * cos_a + targetPos.y * sin_a);
    float c = targetPos.x * targetPos.x + targetPos.y * targetPos.y - radius * radius;

    float D = b*b - 4*a*c;
    float t1 = (-b + sqrt(D))/(2.0f);
    float t2 = (-b - sqrt(D))/(2.0f);
    
    // printf("t1: %.2f, t2: %.2f \n", t1, t2);

    return (Vec3Pair){ {t1*cos_a, t1*sin_a, t1}, {t2*cos_a, t2*sin_a, t2} };
}

Vec3Pair* CalculateCollisionArea(Vector2 targetPos, Vector2 targetVec, float targetRadius, float shipSpeed, int pointsCount)
{
    // float distance = sqrtf(targetX * targetX + targetY * targetY);
    float d = Vector2Length(targetPos); // d - distance to the target
    // printf("Distance: %02f \n", d);
    float m = sqrtf(d * d - targetRadius * targetRadius); // Length of the tangent line (касательной)
    
    float h = targetPos.x, k = targetPos.y; // (h, k) - pos of target
    float r = targetRadius; // - radius of target
    Vector2 F = {m*m*h/(d*d), m*m*k/(d*d)}; // F - Точка пересечения поляры и вектора от нашего корабля к центру окружнсти
    Vector2 s = {-(k*m*r/(d*d)), (h*m*r/(d*d))}; // s - Вектор поляры
 
    // Tangent Lines: 
    T1 = Vector2Add(F, s);
    T2 = Vector2Add(F, Vector2Negate(s));

    if(pointsCount >= 2 && pointsCount % 2 == 0)
    {
        float p1 = atan2(T1.y, T1.x);
        float p2 = atan2(T2.y, T2.x);

        float diff = p1 - p2;
        float shortestDiff = remainder(diff, 2.0f*PI);
        float angleStep = (shortestDiff)/(pointsCount - 1);

        if(IsKeyPressed(KEY_SPACE))
        {
            TraceLog(LOG_INFO, "Start, end, step angle: %.2f, %.2f, %.2f", p1, p2, angleStep);
        }

        Vec3Pair* collisionPoints = (Vec3Pair*)calloc(pointsCount - 1, sizeof(Vec3Pair));
        if(collisionPoints == NULL)
        {
            printf("Unable to create collision points array");
            return NULL;
        }

        for(int i = 1; i < pointsCount - 1; ++i)
            collisionPoints[i] = CalculateIntersection(targetPos, targetRadius, p2 + angleStep * i);
        collisionPoints[0] = (Vec3Pair){{T1.x, T1.y, m}, {T2.x, T2.y, m}};
        
        return collisionPoints;
    }
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib example - basic window");

    int x = 100, y = 100;
    Vector2 shipScreenPos = { WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f };
    Vec3Pair* points;

    int numberOfPoints = 10;
    //Target Parameters
    float r = 50.0f;
    Vector2 mousePos;
    Vector2 targetVec = (Vector2){100.0f, -62.0f};
    Vector2 targetVecScreen = (Vector2){targetVec.x, -targetVec.y};

    while (!WindowShouldClose())
    {
        
        mousePos = GetMousePosition();
        r += GetMouseWheelMove();
        // ---------------------------------------------------------
        // Calculating a Decart positiong of target
        // ---------------------------------------------------------
        Vector2 relativeTargetPosition = {mousePos.x - shipScreenPos.x, -(mousePos.y - shipScreenPos.y)};
        float targetAngle = atan2(relativeTargetPosition.y, relativeTargetPosition.x);
        // Vector2 relativeTargetVec = {targetVec.x, -(targetVec.y)};
        // if(IsKeyPressed(KEY_SPACE))
        //             TraceLog(LOG_INFO, "target vec: %.2f, %.2f", relativeTargetVec.x, relativeTargetVec.y);
        // ---------------------------------------------------------
        // Calculating collision points
        // ---------------------------------------------------------
        points = CalculateCollisionArea(relativeTargetPosition, targetVec, r, 0.0f, numberOfPoints);
        
        //  ---------------------------------------------------------
        //  Calculating the positiong of resulting area on the screen
        //  ---------------------------------------------------------
        Vector2 T1Screen = (Vector2){shipScreenPos.x + T1.x, shipScreenPos.y - T1.y};
        Vector2 T2Screen = (Vector2){shipScreenPos.x + T2.x, shipScreenPos.y - T2.y};
        for(int i = 0; i < numberOfPoints - 1; ++i)
        {
            points[i].first.x = shipScreenPos.x + points[i].first.x;
            points[i].first.y = shipScreenPos.y - points[i].first.y;
            points[i].second.x = shipScreenPos.x + points[i].second.x;
            points[i].second.y = shipScreenPos.y - points[i].second.y;
        }
        // Vector2 col1 = Vec3To2(intersectionPoints.first);
        // Vector2 col2 = Vec3To2(intersectionPoints.second);
        // col1.x = shipScreenPos.x + col1.x;
        // col1.y = shipScreenPos.y - col1.y;
        // col2.x = shipScreenPos.x + col2.x;
        // col2.y = shipScreenPos.y - col2.y;

        // printf("Angle: %.2f \n", targetAngle);
        // Vector2 inters = Vec3To2(intersectionPoints.first);
        // printf("Distance: %.2f | Pos: %.2f, %.2f \n", intersectionPoints.first.z, inters.x, inters.y);
        BeginDrawing();
            DrawText(TextFormat("targetPos: %.2f %.2f", relativeTargetPosition.x, relativeTargetPosition.y), 10, 40, 20, LIGHTGRAY);
            ClearBackground(RAYWHITE);
            DrawCircleV(mousePos, r, RED); 
            DrawLineV(shipScreenPos, T1Screen, GREEN);
            DrawLineV(shipScreenPos, T2Screen, BLUE);
            DrawLineV(mousePos, Vector2Add(mousePos, targetVecScreen), BLACK);

            for(int i = 1; i < numberOfPoints - 1; ++i)
            {
                DrawCircleV(Vec3To2(points[i].first), 5.0f, GREEN);
                DrawCircleV(Vec3To2(points[i].second), 5.0f, BLUE);
            }
            // DrawCircleV(col1, 10.f, GREEN);
            // DrawCircleV(col2, 10.f, BLUE);

            // DrawLineV(shipScreenPos, Vec3To2(intersectionPoints.first), BLACK);
            // DrawLineV(shipScreenPos, Vec3To2(intersectionPoints.second), BLACK);
            // DrawLine(StartPos.x, 0, StartPos.x, WINDOW_HEIGHT, LIGHTGRAY);
            // DrawLine(0, StartPos.y, WINDOW_WIDTH, StartPos.y, LIGHTGRAY);
        EndDrawing();

        if(points != NULL)free(points);
    }

    CloseWindow();

    return 0;
}
