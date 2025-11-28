#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"
using namespace glm;
using namespace std;

struct Collision
{
    vec3         position;
    quat         orientation;
    vec3         half_dimensions;
    vector<vec3> vertices;
    unsigned int VAO;
    Shader       *shader;
    bool         is_colliding;

    Collision(vec3 position, float width, float height, float depth)
    {
        this->position        = position;
        this->half_dimensions = vec3(width/2, height/2, depth/2);
        this->vertices        = get_vertices();
        this->shader          = new Shader("shaders/simple.vert", "shaders/simple.frag");
        this->is_colliding    = false;
        init_vao();
    }

    bool intersects(const Collision &other)
    {
        // this only validates non-rotated bounding boxes
        this->is_colliding = (fabs(position.x - other.position.x) <= (half_dimensions.x + other.half_dimensions.x)) &&
                             (fabs(position.y - other.position.y) <= (half_dimensions.y + other.half_dimensions.y)) &&
                             (fabs(position.z - other.position.z) <= (half_dimensions.z + other.half_dimensions.z));

        return this->is_colliding;
    }

    void render()
    {
        mat4 model = mat4(1.0f);
        model = translate(model, this->position);
        model = model * mat4_cast(this->orientation);
        mat4 view  = context.camera->get_view_matrix();
        mat4 proj  = context.camera->get_perspective_matrix();
        vec3 color = this->is_colliding ? vec3(1.f, 0.f, 0.f) : vec3(0.f, 1.f, 0.f);

        this->shader->use();
        this->shader->set_mat4("model", model);
        this->shader->set_mat4("view", view);
        this->shader->set_mat4("projection", proj);
        this->shader->set_vec3("in_color", color);
        
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_LINES, 0, this->vertices.size());
        glBindVertexArray(0);
    }

    void update(vec3 position, quat orientation)
    {
        this->position    = position;
        this->orientation = orientation;
    }

private:
    vector<vec3> get_vertices()
    {
        vec3 min = -this->half_dimensions;
        vec3 max =  this->half_dimensions;

        vector<vec3> vertices = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {max.x, min.y, min.z}, {max.x, max.y, min.z},
            {max.x, max.y, min.z}, {min.x, max.y, min.z},
            {min.x, max.y, min.z}, {min.x, min.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {max.x, min.y, max.z}, {max.x, max.y, max.z},
            {max.x, max.y, max.z}, {min.x, max.y, max.z},
            {min.x, max.y, max.z}, {min.x, min.y, max.z},

            {min.x, min.y, min.z}, {min.x, min.y, max.z},
            {max.x, min.y, min.z}, {max.x, min.y, max.z},
            {min.x, max.y, min.z}, {min.x, max.y, max.z},
            {max.x, max.y, min.z}, {max.x, max.y, max.z},
        };
        
        return vertices;
    }

    void init_vao()
    {
        unsigned int VBO;

        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(this->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(vec3), this->vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)nullptr);
        glBindVertexArray(0);
    }
};
