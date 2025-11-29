#pragma once
#include "../utils/shader.h"
#include "../context.h"
#include "../utils/camera.h"
#include "../shapes/lightcube.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "../renderer/shadow_map.h"
using namespace glm;
using namespace std;

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 tex_coords;
};

struct Texture
{
    unsigned int id;
    string type, path;
};

struct Mesh
{
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int         VAO, VBO, EBO;

    Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices, const vector<Texture> &textures)
    {
        this->vertices = vertices;
        this->indices  = indices;
        this->textures = textures;

        init();
    }

    void render_shadow_map(Shader *shadow_map_shader, const vec3 position, const quat orientation, const vec3 scalar) const
    {
        mat4 mat_model = mat4(1.0f);
        mat_model = scale(mat_model, scalar);
        mat_model = translate(mat_model, position);
        mat_model = mat_model * mat4_cast(orientation);

        shadow_map_shader->set_mat4("model", mat_model);

        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, (unsigned int)this->indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void render(Shader *shader, const vec3 position, const quat orientation, const vec3 scalar) const
    {
        shader->use();
        shader->set_int("shadow_map", 0);
        shader->set_int("texture", 1);
        shader->set_vec3("camera_pos",  context.camera->position);
        shader->set_vec3("light_pos",   context.light_cube->position);
        shader->set_mat4("light_space_matrix", context.shadow_map->light_space_matrix);
        shader->set_vec3("light_color", vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("mesh_color",  vec3(1.0f, 0.5f, 0.31f));

        mat4 mat_model = mat4(1.0f);
        mat_model = scale(mat_model, scalar);
        mat_model = translate(mat_model, position);
        mat_model = mat_model * mat4_cast(orientation);

        mat4 mat_view = context.camera->get_view_matrix();

        mat4 mat_proj = mat4(1.0f);
        mat_proj = context.camera->get_perspective_matrix();

        shader->set_mat4("model", mat_model);
        shader->set_mat4("view", mat_view);
        shader->set_mat4("projection", mat_proj);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, context.shadow_map->depth_map);

        if (this->textures.size() > 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, this->textures[0].id);
        }

        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, (unsigned int)this->indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

private:
    void init()
    {
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);

        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), this->indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tex_coords));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};
