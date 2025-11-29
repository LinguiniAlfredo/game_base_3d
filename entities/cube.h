#pragma once
#include "entity.h"
#include <glm/glm.hpp>
using namespace glm;

struct Cube : public Entity {

    Cube(const vec3 position = vec3(0.f, 0.f, 0.f),
         const quat orientation = angleAxis(0.f, vec3(0.f, 1.f, 0.f)),
         const float scale = 1.f)
        : Entity("resources/models/cube.obj", position, orientation, scale,
                 "resources/textures/cube.png")
    {
    }
};
