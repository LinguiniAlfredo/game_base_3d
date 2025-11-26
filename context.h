#pragma once
#define MAX_GAMEOBJECTS 50
#include <SDL2/SDL.h>
#include <vector>

typedef struct Cube      Cube;
typedef struct Camera    Camera;
typedef struct LightCube LightCube;
typedef struct Entity    Entity;
typedef struct Floor     Floor;
typedef struct Skybox    Skybox;
typedef struct ShadowMap ShadowMap;

typedef enum {
    MENU,
    GAME,
    PAUSED,
    GAMEOVER,
    EDIT,
    QUIT
} Gamemode;

using namespace std;

typedef struct Context {
    ShadowMap       *shadow_map;
    vector<Cube*>   cubes;
    LightCube       *light_cube;
    Camera          *camera;
    vector<Entity*> entities;
    Floor           *floor;
    Skybox          *skybox;
    Gamemode        mode;
    int             screen_width;
    int             screen_height;
    float           ticks_per_frame;
    int             wireframe;
} Context;

extern Context context;
