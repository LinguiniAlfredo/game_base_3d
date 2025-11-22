#pragma once
#include "model.h"

struct Entity
{
    Model  *model;
    Shader *shader;
    vec3   position;
    vec3   scale;
    //Quaternion orientation;

    Entity(const char *filename, const vec3 position = vec3(0.0f, 0.0f, 0.0f), const float scale = 1.f);
    ~Entity();

    void draw();
};
