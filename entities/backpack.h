#pragma once
#include "model.h"

struct Backpack
{
    Model  *model;
    Shader *shader;
    vec3   position;
    //Quaternion orientation;

    Backpack(const vec3 position);
    ~Backpack();

    void draw();
};
