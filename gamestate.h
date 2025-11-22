#pragma once
#define MAX_GAMEOBJECTS 50
#include <SDL2/SDL.h>
#include <vector>

typedef struct Cube Cube;
typedef struct Camera Camera;
typedef struct LightCube LightCube;
typedef struct Entity Entity;

typedef enum {
    MENU,
    GAME,
    PAUSED,
    GAMEOVER,
    EDIT,
    QUIT
} Gamemode;

using namespace std;

typedef struct Gamestate {
    vector<Cube*>   cubes;
    LightCube       *light_cube;
    Camera          *camera;
    vector<Entity*> entities;
    Gamemode        mode;
    int             screen_width;
    int             screen_height;
    float           ticks_per_frame;
    int             wireframe;
} Gamestate;

extern Gamestate gamestate;
