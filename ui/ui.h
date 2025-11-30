#pragma once
#include <vector>
#include "../utils/shader.h"
#include "glm/glm.hpp"
using namespace glm;

struct UI
{
    unsigned int  VAO, VBO;
    Shader       *shader;
    vector<vec2>  crosshair_vertices;

    UI()
    {
        this->shader             = new Shader("shaders/simple_ui.vert", "shaders/simple_ui.frag");
        this->crosshair_vertices = generate_crosshair_vertices();
        init();
    }

    ~UI()
    {
        delete shader;
        glDeleteVertexArrays(1, &this->VAO);
        glDeleteBuffers(1, &this->VBO);
    }

    void render()
    {
        render_crosshair();
    }

private:
    void init()
    {
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);

        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->crosshair_vertices.size() * sizeof(vec2), this->crosshair_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void render_crosshair()
    {
        glDisable(GL_DEPTH_TEST);

        mat4 model = mat4(1.f);
        mat4 view  = mat4(1.f);
        mat4 proj  = mat4(1.f);
        proj       = ortho(0.f, (float)context.screen_width, 0.f, (float)context.screen_height, -1.0f, 1.0f);

        this->shader->use();
        this->shader->set_vec3("in_color", vec3(1.f, 1.f, 1.f));
        this->shader->set_mat4("model", model);
        this->shader->set_mat4("view", view);
        this->shader->set_mat4("projection", proj);

        glBindVertexArray(this->VAO);
        glDrawArrays(GL_LINES, 0, this->crosshair_vertices.size());
        glBindVertexArray(0);
        
        glEnable(GL_DEPTH_TEST);
    }

    vector<vec2> generate_crosshair_vertices()
    {
        vector<vec2> vertices;
        float center_x = context.screen_width * 0.5f;
        float center_y = context.screen_height * 0.5f;
        float crosshair_size = 10.f;

        vertices.push_back(vec2(center_x, center_y + crosshair_size));
        vertices.push_back(vec2(center_x, center_y - crosshair_size));
        vertices.push_back(vec2(center_x + crosshair_size, center_y));
        vertices.push_back(vec2(center_x - crosshair_size, center_y));

        return vertices;
    }
};
