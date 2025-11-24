#pragma once
#include <glm/glm.hpp>
#include "model.h"
#include "../utils/shader.h"
using namespace glm;

struct Entity
{
    Model  *model;
    Shader *shader;
    vec3   position;
    quat   orientation;
    vec3   scalar;

    Entity();
    Entity(const char *filename,
           const vec3 position = vec3(0.f, 0.f, 0.f),
           const quat orientation = angleAxis(0.f, vec3(0.f, 1.f, 0.f)),
           const float scale = 1.f);
    virtual ~Entity();

    virtual void draw();
};
