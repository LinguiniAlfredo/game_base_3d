#pragma once
#include <glm/glm.hpp>
#include "model.h"

struct Entity
{
    Model  *model;
    Shader *shader;
    vec3   position;
    quat   orientation;
    vec3   scale;

    Entity(const char *filename,
           const vec3 position = vec3(0.0f, 0.0f, 0.0f),
           const quat orientation = angleAxis(0.f, vec3(0.0f, 0.0f, 0.0f)),
           const float scale = 1.f);
    ~Entity();

    void draw();
};
