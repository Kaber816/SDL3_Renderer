#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int POINT_SIZE = 10;

struct point3d {
    float x;
    float y;
    float z;
};

// Original cube points (DO NOT MODIFY)
struct point3d original_points[] = {
    {0, 0, 0},
    {0.5, 0, 0},
    {0, 0.5, 0},
    {0.5, 0.5, 0},
    {0, 0, 0.5},
    {0.5, 0, 0.5},
    {0, 0.5, 0.5},
    {0.5, 0.5, 0.5}
};

// Working buffer (gets rotated each frame)
struct point3d points[8];

struct SDL_FPoint Point_3d_2d(struct point3d *point);
void Rotate_Matrix(struct point3d *points, int count, float angle);

int main() {

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("RENDERER", SCREEN_WIDTH, SCREEN_HEIGHT, 0); 
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    SDL_Event event;
    int isQuitTrue = 0;
    float rotation_angle = 0;

    int point_count = sizeof(original_points) / sizeof(struct point3d);

    while (!isQuitTrue) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isQuitTrue = 1;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

        // Reset points each frame (IMPORTANT)
        memcpy(points, original_points, sizeof(points));

        // Rotate
        Rotate_Matrix(points, point_count, rotation_angle);

        // Convert to 2D
        SDL_FPoint SDL_Points[8];
        for (int i = 0; i < point_count; i++) {
            SDL_Points[i] = Point_3d_2d(&points[i]);
        }

        // Draw points as squares
        SDL_FRect rects[8];
        for (int i = 0; i < point_count; i++) {
            rects[i].x = SDL_Points[i].x - (POINT_SIZE / 2.0f);
            rects[i].y = SDL_Points[i].y - (POINT_SIZE / 2.0f);
            rects[i].w = POINT_SIZE;
            rects[i].h = POINT_SIZE;
        }

        SDL_RenderFillRects(renderer, rects, point_count);

        // Draw lines between points (just sequential for now)
        SDL_RenderLines(renderer, SDL_Points, point_count);

        SDL_RenderPresent(renderer);

        rotation_angle += 1.0f;
        if (rotation_angle > 360.0f) rotation_angle = 0;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

struct SDL_FPoint Point_3d_2d(struct point3d *point) {
    // Simple orthographic projection (no divide by z)
    float x = (point->x + 1.0f) / 2.0f * SCREEN_WIDTH;
    float y = (1.0f - (point->y + 1.0f) / 2.0f) * SCREEN_HEIGHT;

    SDL_FPoint result = {x, y};
    return result;
}

void Rotate_Matrix(struct point3d *points, int count, float angle) {

    float radians = angle * (M_PI / 180.0f);
    float c = cos(radians);
    float s = sin(radians);

    for (int i = 0; i < count; i++) {
        float x = points[i].x;
        float y = points[i].y;

        // Rotate around Z axis
        points[i].x = x * c - y * s;
        points[i].y = x * s + y * c;
    }
}
