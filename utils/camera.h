#pragma once
#include <cstdio>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../context.h"
using namespace glm;

constexpr float YAW         = 90.0f;
constexpr float PITCH       = 0.0f;
constexpr float SPEED       = 10.f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM        = 45.0f;

struct Frustrum {
    float fov;
    float aspect;
    float near, far;
};

struct Camera
{
    vec3 position{};
    vec3 front{};
    vec3 up{};
    vec3 right{};
    vec3 world_up{};
    vec3 trajectory{};
    vec3 input_vector{};

    float yaw;
    float pitch;
    float movement_speed;
    float mouse_sensitivity;
    float zoom;

    Frustrum frustrum{};
 
    Camera(const vec3 position = vec3(0.0f, 0.0f, 0.0f),
           const vec3 up = vec3(0.0f, 1.0f, 0.0f),
           const float yaw = YAW, float pitch = PITCH)
    {
        this->position = position;
        this->world_up = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->front = vec3(0.0f, 0.0f, 0.0f);
        this->movement_speed = SPEED;
        this->mouse_sensitivity = SENSITIVITY;
        this->zoom = ZOOM;
        this->trajectory = vec3(0.0f, 0.0f, 0.0f);
        this->input_vector = vec3(0.0f, 0.0f, 0.0f);
        this->frustrum = {
            .fov    = radians(45.0f),
            .aspect = static_cast<float>(context.screen_width) / static_cast<float>(context.screen_height),
            .near   = 0.1f,
            .far    = 1000.f
        };
        update_camera_vectors();
    }
    virtual ~Camera()
    {

    }

    mat4 get_view_matrix() const
    {
        return lookAt(this->position, this->position + this->front, this->up);
    }

    mat4 get_perspective_matrix() const
    {
        return perspective(frustrum.fov, frustrum.aspect, frustrum.near, frustrum.far);
    }

    virtual void process_keyboard(const SDL_Event &e)
    {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            const SDL_Keycode key = e.key.keysym.sym;
            if (key == SDLK_w)
                this->input_vector.z = 1;
            if (key == SDLK_a)
                this->input_vector.x = 1;
            if (key == SDLK_s)
                this->input_vector.z = -1;
            if (key == SDLK_d)
                this->input_vector.x = -1;
            if (key == SDLK_q)
                this->input_vector.y = -1;
            if (key == SDLK_e)
                this->input_vector.y = 1;
            if (key == SDLK_LSHIFT)
                this->movement_speed = SPEED * 4;
        }
        if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            const SDL_Keycode key = e.key.keysym.sym;
            if (key == SDLK_w)
                this->input_vector.z = 0;
            if (key == SDLK_a)
                this->input_vector.x = 0;
            if (key == SDLK_s)
                this->input_vector.z = 0;
            if (key == SDLK_d)
                this->input_vector.x = 0;
            if (key == SDLK_q)
                this->input_vector.y = 0;
            if (key == SDLK_e)
                this->input_vector.y = 0;
            if (key == SDLK_LSHIFT)
                this->movement_speed = SPEED;
        }
        if (this->input_vector.x != 0 || this->input_vector.y != 0 || this->input_vector.z != 0) {
            this->input_vector = normalize(this->input_vector);
        }
    }

    virtual void update(const float delta_time)
    {
        this->position += this->trajectory * this->movement_speed * delta_time;
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

    void render_crosshair()
    {
        float center_x = context.screen_width / 2;
        float center_y = context.screen_height / 2;
        float crosshair_size = 5;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);  // Set orthographic projection (2D space)
        glMatrixMode(GL_MODELVIEW);

        glBegin(GL_LINES);
        glVertex2f(center_x - crosshair_size, center_y);
        glVertex2f(center_x + crosshair_size, center_y);
        glVertex2f(center_x, center_y - crosshair_size);
        glVertex2f(center_x, center_y + crosshair_size);
        glEnd();
    }

private:
    virtual void update_camera_vectors()
    {
        vec3 f;
        f.x = cos(radians(this->yaw)) * cos(radians(this->pitch));
        f.y = sin(radians(this->pitch));
        f.z = sin(radians(yaw)) * cos(radians(this->pitch));
        this->front = normalize(f);
        this->right = normalize(cross(this->front, this->world_up));
        this->up    = normalize(cross(this->right, this->front));

        this->trajectory = this->input_vector.x * -this->right + this->input_vector.y * this->up + this->input_vector.z * this->front;
    }
};
