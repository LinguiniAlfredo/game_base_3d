#include <cstdio>
#include <cstdint>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"
#include "utils/timer.h"
#include "gamestate.h"
#include "utils/camera.h"
#include "shapes/cube.h"

SDL_Window   *sdl_window = NULL;
SDL_GLContext opengl_context;

Gamestate gamestate = {
    .mode                   = GAME,
    .screen_width           = 1920,
    .screen_height          = 1080,
    .ticks_per_frame        = 1000.f / 144.0f,
    .wireframe              = 0
};

vec3 cube_positions[] = {
    vec3( 0.0f,  0.0f,  0.0f),
    vec3( 2.0f,  5.0f, -15.0f),
    vec3(-1.5f, -2.2f, -2.5f),
    vec3(-3.8f, -2.0f, -12.3f),
    vec3( 2.4f, -0.4f, -3.5f),
    vec3(-1.7f,  3.0f, -7.5f),
    vec3( 1.3f, -2.0f, -2.5f),
    vec3( 1.5f,  2.0f, -2.5f),
    vec3( 1.5f,  0.2f, -1.5f),
    vec3(-1.3f,  1.0f, -1.5f)
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

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, gamestate.screen_width, gamestate.screen_height);

    return 0;
}

void close_app()
{
    for (int i = 0; i < 10; i++) {
        delete gamestate.cubes[i];
    }

    SDL_GL_DeleteContext(opengl_context);
    SDL_DestroyWindow(sdl_window);

    SDL_Quit();
}

void toggle_wireframe()
{
    gamestate.wireframe = !gamestate.wireframe;
    if (gamestate.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void handle_events(float delta_time)
{
    SDL_Event e;
    Camera_Direction direction;
    int mouse_x, mouse_y;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT)
            gamestate.mode = QUIT;

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    gamestate.mode = QUIT;
                    break;
                case SDLK_F1:
                    toggle_wireframe();
                    break;
                case SDLK_w:
                    direction = FORWARD;
                    break;
                case SDLK_a:
                    direction = LEFT;
                    break;
                case SDLK_s:
                    direction = BACKWARD;
                    break;
                case SDLK_d:
                    direction = RIGHT;
                    break;
            }
        }
    }
    gamestate.camera->process_keyboard(direction, delta_time);

    SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
    gamestate.camera->process_mouse((float)mouse_x, -(float)mouse_y, delta_time);
}

void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(int i = 0; i < 10; i++) {
        gamestate.cubes[i]->draw();
    }

    SDL_GL_SwapWindow(sdl_window);
}

void game_loop()
{
    Timer total_timer;
    Timer fps_cap_timer;
    uint64_t current_frame = 0;
    float fps = 0;
    float delta_time = 0;

    timer_start(&total_timer);
    timer_start(&fps_cap_timer);

    while (gamestate.mode != QUIT) {
        fps = current_frame / (timer_get_ticks(&total_timer) / 1000.f);
        current_frame++;

        handle_events(delta_time);

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

        if (fps > 0)
            delta_time = 1 / fps;
        
        printf("FPS: %f\n", fps);
        timer_start(&fps_cap_timer);
    }
}

int main(int argc, char **argv)
{
    if (initialize() == 0) {
        gamestate.camera = new Camera();
        for (int i = 0; i < 10; i++) {
            gamestate.cubes[i] = new Cube(cube_positions[i]);
        }
        game_loop();
    }
    close_app();

    return 0;
}
