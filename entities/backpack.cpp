#include "backpack.h"
#include "floor.h"

void Backpack::update(float delta_time)
{
    // to rotate small amount in one frame, create new quaternion with small angle,
    // then multiply the objects quaternion with the newly created small angle quaternion
    float rotation_speed = 1.f;
    quat delta_rotation = angleAxis(delta_time * rotation_speed, vec3(1.f, 0.f, 0.f));
    this->orientation *= delta_rotation;
    this->orientation = normalize(this->orientation);

    vec3 gravity    = vec3(0.f, -1.f, 0.f);
    float speed     = 1.f;
    vec3 delta      = gravity * speed * delta_time;
    this->position += delta;

    this->collision->update(this->position, this->orientation);

    if (this->collision->intersects(*context.floor->collision)) {
        this->position -= delta;
        this->collision->update(this->position, this->orientation);
    }
}
