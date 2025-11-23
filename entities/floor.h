#pragma once
#include "mesh.h"
#include "../utils/shader.h"
#include <glm/glm.hpp>
#include <vector>
using namespace glm;
using namespace std;

struct Floor
{
    Mesh   mesh;
    Shader *shader;
    vec3   position;

    Floor(const float width, const float length, const vec3 position)
        : mesh(get_vertices(width, length), get_indices())
    {
        this->shader = new Shader("shaders/lighting.vert", "shaders/lighting.frag");
        this->position = position;
    }

    ~Floor()
    {
        delete this->shader;
        glDeleteVertexArrays(1, &this->mesh.VAO);
        glDeleteBuffers(1, &this->mesh.VBO);
        glDeleteBuffers(1, &this->mesh.EBO);
    }

    void draw() const
    {
        // TODO - pass orientation in probably
        const quat orientation = angleAxis(0.f, vec3(0.f, 0.f, 0.f));
        const vec3 scale   = vec3(1.f, 1.f, 1.f);
        this->mesh.draw(this->shader, this->position, orientation, scale);
    }

private:
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
