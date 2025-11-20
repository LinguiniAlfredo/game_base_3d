#pragma once
#define MAX_GAMEOBJECTS 50
#include <SDL2/SDL.h>

typedef struct Cube Cube;
typedef struct Camera Camera;
typedef struct LightCube LightCube;
typedef struct Backpack Backpack;

typedef enum {
    MENU,
    GAME,
    PAUSED,
    GAMEOVER,
    EDIT,
    QUIT
} Gamemode;

typedef struct Gamestate {
    Cube            *cubes[10];
    LightCube       *light_cube;
    Camera          *camera;
    Backpack        *backpack;
    Gamemode        mode;
    int             screen_width;
    int             screen_height;
    float           ticks_per_frame;
    int             wireframe;
} Gamestate;

extern Gamestate gamestate;
