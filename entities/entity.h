#pragma once
#include <glm/glm.hpp>
#include "model.h"
#include "../utils/shader.h"
#include "../utils/collision.h"
using namespace glm;

struct Entity
{
    Model     *model;
    Shader    *shader;
    vec3      position;
    quat      orientation;
    vec3      scalar;
    Collision *collision;

    Entity();
    Entity(const char *filename,
           const vec3 position = vec3(0.f, 0.f, 0.f),
           const quat orientation = angleAxis(0.f, vec3(0.f, 1.f, 0.f)),
           const float scale = 1.f);
    virtual ~Entity();

    virtual void update(float delta_time);
    virtual void render_shadow_map(Shader *shadow_map_shader);
    virtual void render();

    void render_collider();

private:
    vec3 get_dimensions() const;
};
