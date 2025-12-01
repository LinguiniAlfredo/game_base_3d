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
#include "entities/skybox.h"
#include "entities/link.h"
#include "entities/cube.h"
#include "entities/backpack.h"
#include "renderer/shadow_map.h"
#include "utils/player_controller.h"
#include "ui/ui.h"

#include <vector>
#include <typeinfo>

// TODO - create scene class that allocates all entites for that scene
//      - instance all similar static entities liek world_blocks to one draw call
//      - calculate near/far planes for shadow map frustrum
//      - create text renderer (for ui and debug notifications)
//      - weird jittering when walking around an object and keeping cursor on it
//      - camera switching bugs
//      - fix collision stickyness
//      - add pause menu with options for resolution, mouse sensitivity etc.

SDL_Window   *sdl_window = nullptr;
SDL_GLContext opengl_context;

Context context = {
    .mode                   = GAME,
    .screen_width           = 1920,
    .screen_height          = 1080,
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
    delete context.camera;
    delete context.shadow_map;
    delete context.light_cube;
    delete context.skybox;
    delete context.ui;

    for (const auto &world_block : context.world_blocks) {
        delete world_block;
    }
    for (const auto &entity : context.entities) {
        delete entity;
    }

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

void toggle_flycam()
{
    const Camera current_cam = *context.camera;

    if (typeid(*context.camera) != typeid(PlayerController)) {
        delete context.camera;
        context.camera = new PlayerController(current_cam);

    } else if (typeid(*context.camera) == typeid(PlayerController)) {
        delete context.camera;
        context.camera = new Camera(current_cam);
    }
}

void destroy_level()
{
    for (const auto &world_block : context.world_blocks) {
        delete world_block;
    }
    context.world_blocks.clear();

    for (const auto &entity : context.entities) {
        delete entity;
    }
    context.entities.clear();
}

void init_level()
{
    context.entities.push_back(new Link(vec3(5.0f, 20.0f, 0.0f)));
    context.entities.push_back(new Backpack(vec3(-5.0f, 20.0f, 0.0f)));

    // create function for cube grid, pass in dims and origin
    for (int i = 1; i < 20; i+=2) {
        for (int j = 1; j < 20; j+=2) {
            float x = (float)i;
            float z = (float)j;
            context.world_blocks.push_back(new Cube(vec3(x, 0.f, z)));
            context.world_blocks.push_back(new Cube(vec3(-x, 0.f, -z)));
            context.world_blocks.push_back(new Cube(vec3(x, 0.f, -z)));
            context.world_blocks.push_back(new Cube(vec3(-x, 0.f, z)));

            if (x == 9) {
                context.world_blocks.push_back(new Cube(vec3(x, 2.f, z)));
                context.world_blocks.push_back(new Cube(vec3(-x, 2.f, -z)));
                context.world_blocks.push_back(new Cube(vec3(-x, 4.f, -z)));
                context.world_blocks.push_back(new Cube(vec3(-x, 6.f, -z)));
            }
            if (z == 9) {
                context.world_blocks.push_back(new Cube(vec3(x, 2.f, z)));
                context.world_blocks.push_back(new Cube(vec3(-x, 2.f, -z)));
            }
        }
    }
    context.world_blocks.push_back(new Cube(vec3(0.f, 4.f, 0.f)));
    context.world_blocks.push_back(new Cube(vec3(-5.f, 2.f, 5.f)));
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
                case SDLK_F5:
                    toggle_flycam();
                    break;
                case SDLK_1:
                    destroy_level();
                    init_level();
                    break;
            }
        }
        context.camera->process_keyboard(e);
    }
    SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
    context.camera->process_mouse((float)mouse_x, -(float)mouse_y, delta_time);
}

void reset_colliders()
{
    for (const auto &world_block : context.world_blocks) {
        world_block->collision->reset();
    }
    for (const auto &entity : context.entities) {
        entity->collision->reset();
    }
}

void update(const float delta_time)
{
    reset_colliders();

    context.camera->update(delta_time);
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

    if (context.show_shadow_map)
        context.shadow_map->render_depth_quad();

    // draw this last, so fragments behind objects dont get rendered
    // but frig the depth buffer in its shader to pass the depth check
    context.skybox->render();

    // we render ui items last so objects dont occlude them
    context.ui->render();

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

        delta_time = timer_get_ticks(&fps_cap_timer) / 1000.f;
        fps = current_frame / (timer_get_ticks(&total_timer) / 1000.f);
        timer_start(&fps_cap_timer);

        printf("FPS: %f\n", fps);
    }
}


int main(int argc, char **argv)
{
    if (initialize() == 0) {
        context.camera     = new Camera(vec3(0.0f, 10.0f, 20.0f));
        context.shadow_map = new ShadowMap();
        context.light_cube = new LightCube(vec3(-25.0f, 25.0f, -25.0f));
        context.skybox     = new Skybox();
        context.ui         = new UI();

        init_level();

        game_loop();
    }
    close_app();

    return 0;
}
