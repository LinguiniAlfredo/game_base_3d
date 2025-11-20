#include "backpack.h"
#include "model.h"
#include "../utils/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

Backpack::Backpack(const vec3 position)
{
    this->position = position;
    this->shader   = new Shader("shaders/cube.vert", "shaders/cube.frag");
    this->model    = new Model("resources/models/backpack.obj");
}

Backpack::~Backpack()
{
    delete this->shader;
}

void Backpack::draw()
{
    model->draw(this->shader, this->position);
}
