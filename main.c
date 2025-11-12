#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdint.h>
#include "utils/arena.h"
#include "utils/timer.h"
#include "gamestate.h"

SDL_Window   *sdl_window = NULL;
SDL_GLContext opengl_context;

Gamestate gamestate = {
    .mode                   = GAME,
    .screen_width           = 1920/2,
    .screen_height          = 1080/2,
    .ticks_per_frame        = 1000.f / 60,
};

int initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("initialize sdl failed... %s\n", SDL_GetError());
        return 1;
    }

    sdl_window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, gamestate.screen_width,
                                gamestate.screen_height, SDL_WINDOW_OPENGL);
    if (sdl_window == NULL) {
        printf("window could not be created... %s\n", SDL_GetError());
        return 1;
    }

    opengl_context = SDL_GL_CreateContext(sdl_window);
    if (opengl_context == NULL) {
        printf("opengl context could not be created... %s\n", SDL_GetError());
        return 1;
    }

    if (glewInit() != GLEW_OK) {
        printf("glew initialization failed... \n");
        return 1;
    }

    glViewport(0, 0, gamestate.screen_width, gamestate.screen_height);

    return 0;
}

void close_app()
{
    arena_reset(&gamestate.arena);
    arena_destroy(&gamestate.arena);

    SDL_GL_DeleteContext(opengl_context);
    SDL_DestroyWindow(sdl_window);

    SDL_Quit();
}

void handle_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT)
            gamestate.mode = QUIT;

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    gamestate.mode = QUIT;
                    break;
            }
        }
    }
}

void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(sdl_window);
}

void game_loop()
{
    Timer total_timer;
    Timer fps_cap_timer;
    uint64_t current_frame = 0;
    float fps = 0;
    //float delta_time = 0;

    timer_start(&total_timer);
    timer_start(&fps_cap_timer);

    while (gamestate.mode != QUIT) {
        fps = current_frame / (timer_get_ticks(&total_timer) / 1000.f);
        current_frame++;

        handle_events();

        switch (gamestate.mode) {
            case MENU:
                break;
            case GAME:
                render();
                break;
            case PAUSED:
                break;
            case GAMEOVER:
                break;
            case EDIT:
                break;
            case QUIT:
                break;
        }

        int ticks = timer_get_ticks(&fps_cap_timer);
        if (ticks < gamestate.ticks_per_frame)
            SDL_Delay(gamestate.ticks_per_frame - ticks);

        //if (fps > 0)
        //    delta_time = 1 / fps;
        
        printf("FPS: %f\n", fps);
        timer_start(&fps_cap_timer);
    }
}

int main(int argc, char **argv)
{
    arena_create(&gamestate.arena, ARENA_SIZE);

    if (initialize() == 0) {
        game_loop();
    }
    close_app();

    return 0;
}
