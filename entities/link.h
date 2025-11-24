#pragma once
#include "entity.h"

struct Link : public Entity {

    Link(const vec3 position = vec3(0.f, 0.f, 0.f),
         const quat orientation = angleAxis(0.f, vec3(0.f, 1.f, 0.f)),
         const float scale = 1.f)
        : Entity("resources/models/young_link_corrected.fbx",position, orientation, scale)
    {
    }
};
