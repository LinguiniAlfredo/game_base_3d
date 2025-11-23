#pragma once
#include "../utils/shader.h"
#include "../context.h"
#include "../utils/camera.h"
#include "../shapes/lightcube.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
using namespace glm;
using namespace std;

struct Vertex
{
    vec3 position;
    vec3 normal;
};

struct Texture
{
    unsigned int id;
    string type;
};

struct Mesh
{
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    unsigned int         VAO, VBO, EBO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices  = indices;

        setup_mesh();
    }

    void draw(Shader *shader, vec3 position, quat orientation, vec3 scalar)
    {
        shader->use();
        shader->set_vec3("camera_pos",  context.camera->position);
        shader->set_vec3("light_pos",   context.light_cube->position);
        shader->set_vec3("light_color", vec3(1.0f, 1.0f, 1.0f));
        shader->set_vec3("cube_color",  vec3(1.0f, 0.5f, 0.31f));

        mat4 mat_model = mat4(1.0f);
        mat_model = scale(mat_model, scalar);
        mat_model = mat_model * mat4_cast(orientation);
        mat_model = translate(mat_model, position);

        mat_model = rotate(mat_model, SDL_GetTicks64()/1000.f, vec3(0.0f, 1.0f, 0.0f));

        mat4 mat_view = context.camera->get_view_matrix();

        mat4 mat_proj = mat4(1.0f);
        mat_proj = context.camera->get_perspective_matrix();

        shader->set_mat4("model", mat_model);
        shader->set_mat4("view", mat_view);
        shader->set_mat4("projection", mat_proj);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    void setup_mesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
};
