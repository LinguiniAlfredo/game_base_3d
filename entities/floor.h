#pragma once
#include "mesh.h"
#include "../utils/shader.h"
#include <glm/glm.hpp>
#include <vector>
using namespace glm;
using namespace std;

struct Floor
{
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    Shader               *shader;
    vec3                 position;
    unsigned int         VAO, VBO, EBO;

    Floor(const float width, const float length, const vec3 position)
    {
        this->vertices = get_vertices(width, length);
        this->indices  = get_indices();
        this->shader   = new Shader("shaders/lighting.vert", "shaders/lighting.frag");
        this->position = position;
        init();
    }

    ~Floor()
    {
        delete this->shader;
        glDeleteVertexArrays(1, &this->VAO);
        glDeleteBuffers(1, &this->VBO);
        glDeleteBuffers(1, &this->EBO);
    }

    void render_shadow_map(Shader *shadow_map_shader)
    {
        // TODO - pass orientation in probably
        const quat orientation = angleAxis(0.f, vec3(0.f, 0.f, 0.f));
        const vec3 scalar      = vec3(1.f, 1.f, 1.f);

        shadow_map_shader->use();

        mat4 mat_model = mat4(1.0f);
        mat_model = scale(mat_model, scalar);
        mat_model = mat_model * mat4_cast(orientation);
        mat_model = translate(mat_model, position);

        shadow_map_shader->set_mat4("model", mat_model);

        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, (unsigned int)this->indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void render() const
    {
        // TODO - pass orientation in probably
        const quat orientation = angleAxis(0.f, vec3(0.f, 0.f, 0.f));
        const vec3 scalar      = vec3(1.f, 1.f, 1.f);

        this->shader->use();
        this->shader->set_vec3("camera_pos",  context.camera->position);
        this->shader->set_vec3("light_pos",   context.light_cube->position);
        this->shader->set_vec3("light_color", vec3(1.0f, 1.0f, 1.0f));
        this->shader->set_vec3("mesh_color",  vec3(1.0f, 0.5f, 0.31f));

        mat4 mat_model = mat4(1.0f);
        mat_model = scale(mat_model, scalar);
        mat_model = mat_model * mat4_cast(orientation);
        mat_model = translate(mat_model, position);

        mat4 mat_view = context.camera->get_view_matrix();

        mat4 mat_proj = mat4(1.0f);
        mat_proj = context.camera->get_perspective_matrix();

        shader->set_mat4("model", mat_model);
        shader->set_mat4("view", mat_view);
        shader->set_mat4("projection", mat_proj);

        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

private:
    void init()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    static vector<Vertex> get_vertices(float width, float length)
    {
        vector<Vertex> vertices;
        Vertex         vertex{};
        vec3           vector;
        
        vector = vec3(-width, 0.f, length);
        vertex.position = vector;
        vertex.normal   = vec3(0.f, 1.f, 0.f); // TODO - base normal off orientation
        vertices.push_back(vertex);

        vector = vec3(width, 0.f, length);
        vertex.position = vector;
        vertex.normal   = vec3(0.f, 1.f, 0.f);
        vertices.push_back(vertex);

        vector = vec3(width, 0.f, -length);
        vertex.position = vector;
        vertex.normal   = vec3(0.f, 1.f, 0.f);
        vertices.push_back(vertex);

        vector = vec3(-width, 0.f, -length);
        vertex.position = vector;
        vertex.normal   = vec3(0.f, 1.f, 0.f);
        vertices.push_back(vertex);

        return vertices;
    }

    static vector<unsigned int> get_indices()
    {
        vector<unsigned int> indices;

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);

        return indices;
    }
};
