#pragma once
#define MAX_GAMEOBJECTS 50
#include <SDL2/SDL.h>
#include <vector>

typedef struct Cube      Cube;
typedef struct Camera    Camera;
typedef struct LightCube LightCube;
typedef struct Entity    Entity;
typedef struct Skybox    Skybox;
typedef struct ShadowMap ShadowMap;
typedef struct UI        UI;

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
    Skybox          *skybox;
    LightCube       *light_cube;
    Camera          *camera;
    UI              *ui;
    vector<Entity*>  world_blocks;
    vector<Entity*>  entities;
    Gamemode         mode;
    int              screen_width;
    int              screen_height;
    float            ticks_per_frame;
    bool             wireframe;
    bool             show_shadow_map;
    bool             show_collisions;
} Context;

extern Context context;
