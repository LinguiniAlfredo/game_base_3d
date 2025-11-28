#include <cstdio>
#include <cstdint>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"

#include "utils/timer.h"
#include "context.h"
#include "utils/camera.h"
#include "shapes/lightcube.h"
#include "entities/entity.h"
#include "entities/floor.h"
#include "entities/skybox.h"
#include "entities/link.h"
#include "entities/backpack.h"
#include "entities/cube.h"
#include "renderer/shadow_map.h"
#include "utils/collision.h"
#include <vector>

SDL_Window   *sdl_window = nullptr;
SDL_GLContext opengl_context;

Context context = {
    .mode                   = GAME,
    .screen_width           = 1920/2,
    .screen_height          = 1080/2,
    .ticks_per_frame        = 1000.f / 144.0f,
    .wireframe              = false,
    .show_shadow_map        = false,
    .show_collisions        = false
};

vec3 positions[] = {
    vec3(1.0f, 0.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f),
    vec3(-1.0f, 0.0f, 0.0f),
    vec3(0.0f, 0.0f, -1.0f),
};

int initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("initialize sdl failed... %s\n", SDL_GetError());
        return 1;
    }

    sdl_window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, context.screen_width,
                                context.screen_height, SDL_WINDOW_OPENGL);
    if (sdl_window == nullptr) {
        printf("window could not be created... %s\n", SDL_GetError());
        return 1;
    }

    opengl_context = SDL_GL_CreateContext(sdl_window);
    if (opengl_context == nullptr) {
        printf("opengl context could not be created... %s\n", SDL_GetError());
        return 1;
    }

    if (glewInit() != GLEW_OK) {
        printf("glew initialization failed... \n");
        return 1;
    }

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetWindowGrab(sdl_window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, context.screen_width, context.screen_height);

    return 0;
}

void close_app()
{
    delete context.light_cube;

    for (const auto &world_block : context.world_blocks) {
        delete world_block;
    }

    for (const auto &entity : context.entities) {
        delete entity;
    }

    delete context.floor;
    delete context.skybox;

    SDL_GL_DeleteContext(opengl_context);
    SDL_DestroyWindow(sdl_window);

    SDL_Quit();
}

void toggle_wireframe()
{
    context.wireframe = !context.wireframe;
    if (context.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void toggle_shadow_map()
{
    context.show_shadow_map = !context.show_shadow_map;
}

void toggle_collision_render()
{
    context.show_collisions = !context.show_collisions;
}

void toggle_paused()
{
    if (context.mode == PAUSED) {
        context.mode = GAME;
    } else {
        context.mode = PAUSED;
    }
}

void handle_events(const float delta_time)
{
    SDL_Event e;
    int mouse_x, mouse_y;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT)
            context.mode = QUIT;

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    context.mode = QUIT;
                    break;
                case SDLK_TAB:
                    toggle_paused();
                    break;
                case SDLK_F1:
                    toggle_wireframe();
                    break;
                case SDLK_F2:
                    toggle_shadow_map();
                    break;
                case SDLK_F3:
                    toggle_collision_render();
                    break;
            }
        }
        context.camera->process_keyboard(e);
    }
    SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
    context.camera->process_mouse((float)mouse_x, -(float)mouse_y, delta_time);

    context.camera->move(delta_time);
}

void update(const float delta_time)
{
    for (const auto &world_block : context.world_blocks) {
        world_block->update(delta_time);
    }
    for (const auto &entity : context.entities) {
        entity->update(delta_time);

    }
}

void render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    context.shadow_map->do_pass();

    for (const auto &world_block : context.world_blocks) {
        world_block->render();
        if (context.show_collisions)
            world_block->render_collider();
    }

    context.light_cube->render();
    for (const auto &entity : context.entities) {
        entity->render();
        if (context.show_collisions)
            entity->render_collider();
    }

    context.floor->render();
    if (context.show_collisions) {
        context.floor->collision->render();
    }

    if (context.show_shadow_map)
        context.shadow_map->render_depth_quad();

    // draw this last, so fragments behind objects dont get rendered
    // but frig the depth buffer in its shader to pass the depth check
    context.skybox->render();

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

    while (context.mode != QUIT) {
        fps = current_frame / (timer_get_ticks(&total_timer) / 1000.f);
        current_frame++;

        handle_events(delta_time);

        switch (context.mode) {
            case MENU:
                break;
            case GAME:
                update(delta_time);
                render();
                break;
            case PAUSED:
                render();
                break;
            case GAMEOVER:
                break;
            case EDIT:
                break;
            case QUIT:
                break;
        }

        int ticks = timer_get_ticks(&fps_cap_timer);
        if (ticks < context.ticks_per_frame)
            SDL_Delay(context.ticks_per_frame - ticks);

        if (fps > 0)
            delta_time = 1 / fps;
        
        //printf("FPS: %f\n", fps);
        timer_start(&fps_cap_timer);
    }
}

int main(int argc, char **argv)
{
    if (initialize() == 0) {
        context.camera = new Camera(vec3(0.0f, 0.0f, -5.0f));
        context.shadow_map = new ShadowMap();
        context.light_cube = new LightCube(vec3(-50.0f, 50.0f, -50.0f));
        context.entities.push_back(new Backpack(vec3(-5.0f, 0.0f, -5.0f)));
        context.entities.push_back(new Link(vec3(0.0f, 0.0f, 0.0f)));
        context.floor  = new Floor(100.f, 100.f, vec3(0.f, -5.f, 0.f));

        context.world_blocks.push_back(new Cube(vec3(-10.f, 0.f, 0.f)));

        context.skybox = new Skybox();

        game_loop();
    }
    close_app();

    return 0;
}
