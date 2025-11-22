#include "entity.h"
#include "model.h"
#include "../utils/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

Entity::Entity(const char *filename, const vec3 position, const float scale)
{
    this->position = position;
    this->scale    = vec3(scale, scale, scale);
    this->shader   = new Shader("shaders/cube.vert", "shaders/cube.frag");
    this->model    = new Model(filename);
}

Entity::~Entity()
{
    delete this->shader;
    delete this->model;
}

void Entity::draw()
{
    model->draw(this->shader, this->position, this->scale);
}
