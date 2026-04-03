#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int POINT_SIZE = 10;
const int FOCAL_LENGTH = 90;
float distance = 10.0f;

struct point3d {
    float x;
    float y;
    float z;
};

struct edge {
    SDL_FPoint point1;
    SDL_FPoint point2;
};

/*
 * x' = x/z
 * y' = y/z
 * 0,0 is at center
 * range of equation is -1 , 1 on both sides
*/

struct SDL_FPoint Point_3d_To_Screenspace(struct point3d *point);

void Rotate_Points_Y_Axis(struct point3d *points, size_t count, float angle);
void Rotate_Points_X_Axis(struct point3d *points, size_t count, float angle);

int main(int argc, char **argv) {

// ----------------------
// --- File Processing
// ----------------------

    if (argc > 2) {
        fprintf(stderr, "Too many arguments passed.\n");
        return 1;
    } else if (argc < 2) {
        fprintf(stderr, "Too few arguments.\n");
        return 1;
    }
    
    FILE *obj; // File to store obj

    // Open obj file
    obj = fopen(argv[1], "r");

    // Return error if file open fails
    if (obj == NULL) {
        fprintf(stderr, "Error opening obj file\n");
        return 1; 
    }
 
    char buffer[1024];
    ssize_t read = 0;

    char c = fgetc(obj);
    while (c == '#') {
        //printf("%c", c);
        while (c != '\n') {
            c = fgetc(obj);
            //printf("%c", c);
        }
        c = fgetc(obj);
    }

    ungetc(c, obj); // Unget the first char
    char *token = "";
    long data_start_pos = ftell(obj); // Store the file pointer position of start of data
    int points_count = 0;

    while (strcmp(token, "f") != 0) {
        fgets(buffer, 50, obj);
        token = strtok(buffer, " ");

        if(strcmp(token, "v") == 0) {
            points_count++;
            //printf("%d\n", points_count);
        }
    }
    printf("Vertices: %d\n", points_count);

    int x_val;
    int y_val;
    int z_val;
    int curr_point = 0;
    struct point3d points[points_count];
    
    // Move fp back to beginning of data
    fseek(obj, data_start_pos, SEEK_SET);

    // Store points in array
    while (curr_point < points_count) {
        fgets(buffer, 50, obj);
        token = strtok(buffer, " ");

        if(strcmp(token, "v") == 0) {
            // X
            token = strtok(NULL, " ");
            x_val = atof(token);

            // Y
            token = strtok(NULL, " ");
            y_val = atof(token);

            // Z
            token = strtok(NULL, " ");
            z_val = atof(token);

            struct point3d temp = {x_val, y_val, z_val};
            points[curr_point] = temp;
            curr_point++;
        }
    }

    // Get edges to draw lines using face values
    while (c != 'f') {
        //printf("%c", c);
        while (c != '\n') {
            c = fgetc(obj);
            //printf("%c", c);
        }
        c = fgetc(obj);
    }

    ungetc(c, obj); // Unget the first char, should be at first face value now
    
    long faces_start_pos = ftell(obj); // Store the file pointer position of start of data
    int edges_count = 0;
    
    while (*fgets(buffer, 50, obj) != EOF) {
        token = strtok(buffer, " ");

        if(strcmp(token, "f") == 0) {
            edges_count++;
            printf("%d\n", edges_count);
        }
    }

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

    //int points_count = sizeof(points) / sizeof(struct point3d);
    bool right_click = false;

    // Main loop
    while (!isQuitTrue) {

       // Event handling to close program
        while (SDL_PollEvent(&event)) {

            // Detect quit event
            if (event.type == SDL_EVENT_QUIT) {
                isQuitTrue = 1;
                printf("QUIT DETECTED\n");
                break;
            }
            
            // Enable scrolling for zoom in and out
            if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                if (event.wheel.y > 0) {
                    distance -= 0.1;
                }

                if (event.wheel.y < 0) {
                    distance += 0.1;
                }
            }

            // Enable infinite mouse movement on right click, undo when released
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                
                if (event.button.button == SDL_BUTTON_RIGHT) {

                    // Allow mouse to move infinitely
                    SDL_SetWindowRelativeMouseMode(window, true);
                    right_click = true;
                }
            }
            
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    // Allow mouse to move infinitely
                    SDL_SetWindowRelativeMouseMode(window, false);
                    right_click = false;
                }
            }
            
            // Enable object rotation with mouse
            if (event.type == SDL_EVENT_MOUSE_MOTION && right_click) {
                Rotate_Points_X_Axis(points, points_count, -event.motion.yrel);
                Rotate_Points_Y_Axis(points, points_count, -event.motion.xrel);
            }
        }
        
        // Set background to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);
    
        // Set draw color to white for pixels
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        // Rotate points constantly
        //Rotate_Points_X_Axis(points, points_count, 0.001);
        //Rotate_Points_Y_Axis(points, points_count, 0.001);

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
            float point_size = POINT_SIZE / (points[i].z + distance);
            struct SDL_FRect curr_rect = {curr_point.x - (point_size / 2.0), curr_point.y - (point_size / 2.0), point_size, point_size};
            rects[i] = curr_rect;
        }

      // Render rects
        SDL_RenderFillRects(renderer, rects, points_count);

        // Create lines 
        //int edges_count = sizeof(edges) / sizeof(struct edge);
        //for (int i = 0; i < edges_count; i++) {
        //    SDL_RenderLine(renderer, edges[i].point1.x, edges[i].point1.y, edges[i].point2.x, edges[i].point2.y);    
        //}
        //SDL_RenderLines(renderer, SDL_Points, points_count);

        // Present renderer
        SDL_RenderPresent(renderer); 


    }

    // Destroy SDL stuff and free memory
    fclose(obj);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}

struct SDL_FPoint Point_3d_To_Screenspace(struct point3d *point) {

    float z = 1 / (point->z + distance);  // prevent divide by zero and also minimize division

    float x = (point->x * FOCAL_LENGTH) * z;
    float y = (point->y * FOCAL_LENGTH) * z;

    // center on screen
    x += SCREEN_WIDTH / 2.0f;
    y = SCREEN_HEIGHT / 2.0f - y;

    return (SDL_FPoint){x, y};
}

void Rotate_Points_Y_Axis(struct point3d *points, size_t count, float angle) {

    float r = angle * (M_PI / 180.0f);

    float c = cos(r);
    float s = sin(r);

    for (int p = 0; p < count; p++) {
        float x = points[p].x;
        float z = points[p].z;
        
        // Was slightly confused by this at first, looking at coordinate plane from screen point of view,
        // -Z is towards user, +Z is away, +x is right of the screen, +y is top of the screen
        // so here we rotate around Y which makes it look like it's rotating left
        points[p].x = (x * c) + (z * s); 
        points[p].z = (-x * s) + (z * c); 
    }
}

void Rotate_Points_X_Axis(struct point3d *points, size_t count, float angle) {

    float r = angle * (M_PI / 180.0f);

    float c = cos(r);
    float s = sin(r);

    for (int p = 0; p < count; p++) {
        float y = points[p].y;
        float z = points[p].z;
        
        points[p].y = (y * c) + (z * -s); 
        points[p].z = (y * s) + (z * c); 
    }
}

