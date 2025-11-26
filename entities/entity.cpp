#include "entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

Entity::Entity()
{
    this->position    = vec3(0.f, 0.f, 0.f);
    this->orientation = angleAxis(0.f, vec3(0.f, 1.f, 0.f));
    this->scalar      = vec3(1.f, 1.f, 1.f);
    this->shader      = nullptr;
    this->model       = nullptr;
}

Entity::Entity(const char *filename, const vec3 position, const quat orientation, const float scale)
{
    this->position    = position;
    this->orientation = orientation;
    this->scalar      = vec3(scale, scale, scale);
    this->shader      = new Shader("shaders/lighting.vert", "shaders/lighting.frag");
    this->model       = new Model(filename);
}

Entity::~Entity()
{
    delete this->shader;
    delete this->model;
}

void Entity::update(float delta_time)
{

}

void Entity::render_shadow_map(Shader *shadow_map_shader)
{
    model->render_shadow_map(shadow_map_shader, this->position, this->orientation, this->scalar);
}

void Entity::render()
{
    model->draw(this->shader, this->position, this->orientation, this->scalar);
}
