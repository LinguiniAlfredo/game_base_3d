#include "link.h"

void Link::update(float delta_time)
{
    // to rotate small amount in one frame, create new quaternion with small angle,
    // then multiply the objects quaternion with the newly created small angle quaternion
    //float rotation_speed = 1.f;
    //quat delta_rotation = angleAxis(delta_time * rotation_speed, vec3(0.f, 1.f, 0.f));
    //this->target_orientation *= delta_rotation;
    //this->target_orientation  = normalize(this->target_orientation);

    vec3 gravity          = vec3(0.f, -1.f, 0.f);
    float speed           = 9.8;
    this->target_position += gravity * speed * delta_time;

    Entity::update(delta_time);
}
