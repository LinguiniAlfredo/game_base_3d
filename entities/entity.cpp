#include "entity.h"
#include "floor.h"
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
    this->collision   = nullptr;

    this->target_position = this->position;
    this->target_orientation = this->orientation;
}

Entity::Entity(const char *filename, const vec3 position, const quat orientation, const float scale)
{
    this->position           = position;
    this->target_position    = position;
    this->orientation        = orientation;
    this->target_orientation = orientation;
    this->scalar             = vec3(scale, scale, scale);
    this->shader             = new Shader("shaders/lighting.vert", "shaders/lighting.frag");
    this->model              = new Model(filename);

    vec3 dimensions = get_dimensions();
    this->collision   = new Collision(position, dimensions.x, dimensions.y, dimensions.z);
}

Entity::~Entity()
{
    delete this->shader;
    delete this->model;
    delete this->collision;
}

void Entity::update(float delta_time)
{
    this->collision->update(this->target_position, this->target_orientation);
    if (this->collision->intersects(*context.floor->collision)) {
        this->collision->update(this->position, this->orientation);
        this->target_position = this->position;
        this->target_orientation = this->orientation;
    } else {
        this->position    = this->target_position;
        this->orientation = this->target_orientation;
    }
}

void Entity::render_shadow_map(Shader *shadow_map_shader)
{
    this->model->render_shadow_map(shadow_map_shader, this->position, this->orientation, this->scalar);
}

void Entity::render()
{
    this->model->render(this->shader, this->position, this->orientation, this->scalar);
}

void Entity::render_collider()
{
    this->collision->render();
}

vec3 Entity::get_dimensions() const
{
    float min_x = 0.f, max_x = 0.f;
    float min_y = 0.f, max_y = 0.f;
    float min_z = 0.f, max_z = 0.f;

    for (unsigned int i = 0; i < this->model->meshes.size(); i++) {
        Mesh mesh = this->model->meshes[i];
        for (unsigned int j = 0; j < mesh.vertices.size(); j++) {
            Vertex vertex = mesh.vertices[j];
            if (vertex.position.x < min_x) {
                min_x = vertex.position.x;
            }
            if (vertex.position.y < min_y) {
                min_y = vertex.position.y;
            }
            if (vertex.position.z < min_z) {
                min_z = vertex.position.z;
            }
            if (vertex.position.x > max_x) {
                max_x = vertex.position.x;
            }
            if (vertex.position.y > max_y) {
                max_y = vertex.position.y;
            }
            if (vertex.position.z > max_z) {
                max_z = vertex.position.z;
            }
        }
    }
    return vec3(max_x - min_x, max_y - min_y, max_z - min_z);
}
