#pragma once
#include <cstdio>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

const float YAW         = 90.0f;
const float PITCH       = 0.0f;
const float SPEED       = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM        = 45.0f;

struct Camera
{
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;
    vec3 trajectory;

    float yaw;
    float pitch;
    float movement_speed;
    float mouse_sensitivity;
    float zoom;
 
    Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f),
           vec3 up = vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH)
    {
        this->position = position;
        this->world_up = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->front = vec3(0.0f, 0.0f, -1.0f);
        this->movement_speed = SPEED;
        this->mouse_sensitivity = SENSITIVITY;
        this->zoom = ZOOM;
        this->trajectory = vec3(0.0f, 0.0f, 0.0f);
        update_camera_vectors();
    }

    mat4 get_view_matrix()
    {
        return lookAt(this->position, this->position + this->front, this->up);
    }

    void process_keyboard(SDL_Event e)
    {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            SDL_Keycode key = e.key.keysym.sym;
            if (key == SDLK_w)
                this->trajectory.z = 1;
            if (key == SDLK_a)
                this->trajectory.x = 1;
            if (key == SDLK_s)
                this->trajectory.z = -1;
            if (key == SDLK_d)
                this->trajectory.x = -1;
            if (key == SDLK_q)
                this->trajectory.y = -1;
            if (key == SDLK_e)
                this->trajectory.y = 1;
        }
        if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            SDL_Keycode key = e.key.keysym.sym;
            if (key == SDLK_w)
                this->trajectory.z = 0;
            if (key == SDLK_a)
                this->trajectory.x = 0;
            if (key == SDLK_s)
                this->trajectory.z = 0;
            if (key == SDLK_d)
                this->trajectory.x = 0;
            if (key == SDLK_q)
                this->trajectory.y = 0;
            if (key == SDLK_e)
                this->trajectory.y = 0;
        }
        if (this->trajectory.x != 0 || this->trajectory.y != 0 || this->trajectory.z != 0) {
            this->trajectory = normalize(this->trajectory);
        }
    }

    void move(float delta_time)
    {
        float velocity = this->movement_speed * delta_time;
        this->position += this->trajectory * velocity;
    }

    void process_mouse(float x_offset, float y_offset, float delta_time, GLboolean constrain_pitch = true)
    {
        x_offset *= this->mouse_sensitivity;
        y_offset *= this->mouse_sensitivity;

        this->yaw   += x_offset;
        this->pitch += y_offset;

        if (constrain_pitch) {
            if (this->pitch > 89.0f)
                this->pitch = 89.0f;
            if (this->pitch < -89.0f)
                this->pitch = -89.0f;
        }

        update_camera_vectors();
    }

    void process_mouse_scroll(float y_offset)
    {
        this->zoom -= (float)y_offset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

private:
    void update_camera_vectors()
    {
        vec3 front;
        front.x = cos(radians(this->yaw)) * cos(radians(this->pitch));
        front.y = sin(radians(this->pitch));
        front.z = sin(radians(yaw)) * cos(radians(this->pitch));
        this->front = normalize(front);
        this->right = normalize(cross(this->front, this->world_up));
        this->up    = normalize(cross(this->right, this->front));
    }
};
