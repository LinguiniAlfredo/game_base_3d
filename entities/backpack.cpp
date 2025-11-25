#include "backpack.h"

void Backpack::update(float delta_time)
{
    // to rotate small amount in one frame, create new quaternion with small angle,
    // then multiply the objects quaternion with the newly created small angle quaternion
    float rotation_speed = 1.f;
    quat delta_rotation = angleAxis(delta_time * rotation_speed, vec3(1.f, 0.f, 0.f));
    this->orientation *= delta_rotation;
    this->orientation = normalize(this->orientation);
}
