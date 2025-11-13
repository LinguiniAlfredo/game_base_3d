#pragma once
#define MAX_GAMEOBJECTS 50
#include <SDL2/SDL.h>

typedef struct Triangle Triangle;
typedef struct Rectangle Rectangle;

typedef enum {
    MENU,
    GAME,
    PAUSED,
    GAMEOVER,
    EDIT,
    QUIT
} Gamemode;

typedef struct Gamestate {
    //Entity       *entities[MAX_GAMEOBJECTS];
    Rectangle    *rectangle;
    Triangle     *triangle;
    Arena         arena;
    Gamemode      mode;
    int           screen_width;
    int           screen_height;
    float         ticks_per_frame;
    int           wireframe;
} Gamestate;

extern Gamestate gamestate;
