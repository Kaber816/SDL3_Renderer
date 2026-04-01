#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int POINT_SIZE = 10;
const int FOCAL_LENGTH = 90;

struct point3d {
    float x;
    float y;
    float z;
};

struct point3d points[] = {
    {-0.25, -0.25, 0.25},
    {-0.25, 0.25, 0.25},
    {0.25, -0.25, 0.25},
    {0.25, 0.25, 0.25},
    {-0.25, -0.25, -0.25},
    {-0.25, 0.25, -0.25},
    {0.25, -0.25, -0.25},
    {0.25, 0.25, -0.25},
    {-0.5, 0.5, -20}
};

/*
 * x' = x/z
 * y' = y/z
 * 0,0 is at center
 * range of equation is -1 , 1 on both sides
*/

struct SDL_FPoint Point_3d_To_Screenspace(struct point3d *point);

void Rotate_Points(struct point3d *points, size_t count, float angle);

int main() {

// ----------------------
// --- SDL Initialization
// ----------------------

    // SDL Initalizations
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("RENDERER", SCREEN_WIDTH, SCREEN_HEIGHT, 0); 
    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Loop variables
    SDL_Event event;
    int isQuitTrue = 0;
    int points_count = sizeof(points) / sizeof(struct point3d);

    // Main loop
    while (!isQuitTrue) {
       // Event handling to close program
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isQuitTrue = 1;
                printf("QUIT DETECTED\n");
                break;
            }
        }
        
        // Set background to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);
    
        // Set draw color to white for pixels
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        // Rotate points
        //Rotate_Points(points, points_count, 0.01);

        SDL_FPoint SDL_Points[points_count];
        // Make array for SDL style points
        for (int i = 0; i < points_count; i++) {
            SDL_FPoint curr_point = Point_3d_To_Screenspace(&points[i]);
            SDL_Points[i] = curr_point;
        }

        SDL_FRect rects[points_count];
        // Go through SDL_FPoint and make lists
        for (int i = 0; i < points_count; i++) {
            SDL_FPoint curr_point = SDL_Points[i];
            struct SDL_FRect curr_rect = {curr_point.x - (POINT_SIZE / 2.0), curr_point.y - (POINT_SIZE / 2.0), POINT_SIZE, POINT_SIZE};
            rects[i] = curr_rect;
        }

        // Render rects
        SDL_RenderFillRects(renderer, rects, points_count);

        // Create lines
        //SDL_RenderLines(renderer, SDL_Points, sizeof(SDL_Points) / sizeof(SDL_FPoint));
        SDL_RenderPresent(renderer); 


    }

    // Destroy SDL stuff and free memory
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}

struct SDL_FPoint Point_3d_To_Screenspace(struct point3d *point) {

    float x = (point->x * FOCAL_LENGTH) / point->z;
    float y = (point->y * FOCAL_LENGTH) / point->z;
    
    x = (point->x + 1) / 2 * SCREEN_WIDTH;
    y = (1 - (point->y + 1)/2) * SCREEN_HEIGHT;
        
    SDL_FPoint point_prime = {x,y};
    
    return point_prime;
}

void Rotate_Points(struct point3d *points, size_t count, float angle) {
    
    float r = angle * (M_PI / 180.0f);

    float c = cos(r);
    float s = sin(r);

    for (int p = 0; p < count; p++) {
        float x = points[p].x;
        float y = points[p].y;

        points[p].x = (x * c) - (y * s); 
        points[p].y = (x * s) + (y * c); 
    }
}
