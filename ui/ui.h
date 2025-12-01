#pragma once
#include <vector>
#include "../utils/shader.h"
#include "glm/glm.hpp"
#include "../entities/mesh.h"
using namespace glm;


struct UI
{
    unsigned int  crosshair_VAO, crosshair_VBO;
    Shader       *shader;
    vector<vec2>  crosshair_vertices;

    unsigned int  pause_VAO, pause_VBO;
    vector<vec2>  pause_quad_vertices;

    UI()
    {
        this->shader              = new Shader("shaders/simple_ui.vert", "shaders/simple_ui.frag");
        this->crosshair_vertices  = generate_crosshair_vertices();
        this->pause_quad_vertices = generate_quad_vertices();

        init();
    }

    ~UI()
    {
        delete shader;
        glDeleteVertexArrays(1, &this->crosshair_VAO);
        glDeleteBuffers(1, &this->crosshair_VBO);
        glDeleteVertexArrays(1, &this->pause_VAO);
        glDeleteBuffers(1, &this->pause_VBO);
    }

    void render()
    {
        if (context.mode == PAUSED){
            render_pause_menu();
        }
        else {
            render_crosshair();
        }
    }

private:
    void init()
    {
        // crosshair init
        glGenVertexArrays(1, &this->crosshair_VAO);
        glGenBuffers(1, &this->crosshair_VBO);

        glBindVertexArray(this->crosshair_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->crosshair_VBO);
        glBufferData(GL_ARRAY_BUFFER, this->crosshair_vertices.size() * sizeof(vec2), this->crosshair_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        // pause menu init TODO - create helper functions to do all this shit!
        glGenVertexArrays(1, &this->pause_VAO);
        glGenBuffers(1, &this->pause_VBO);

        glBindVertexArray(this->pause_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->pause_VBO);
        glBufferData(GL_ARRAY_BUFFER, this->pause_quad_vertices.size() * sizeof(vec2), this->pause_quad_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void render_crosshair()
    {
        mat4 model = mat4(1.f);
        mat4 view  = mat4(1.f);
        mat4 proj  = mat4(1.f);
        proj       = ortho(0.f, (float)context.screen_width, 0.f, (float)context.screen_height, -1.0f, 1.0f);

        this->shader->use();
        this->shader->set_vec4("in_color", vec4(1.0f, 1.0f, 1.0f, 1.0f));
        this->shader->set_mat4("model", model);
        this->shader->set_mat4("view", view);
        this->shader->set_mat4("projection", proj);

        glBindVertexArray(this->crosshair_VAO);
        glDrawArrays(GL_LINES, 0, this->crosshair_vertices.size());
        glBindVertexArray(0);
    }

    vector<vec2> generate_crosshair_vertices()
    {
        vector<vec2> vertices;
        float center_x = context.screen_width * 0.5f;
        float center_y = context.screen_height * 0.5f;
        float line_size = 6.f;
        float center_size = 4.f;

        vertices.push_back(vec2(center_x, center_y + center_size));
        vertices.push_back(vec2(center_x, center_y + center_size + line_size));
        vertices.push_back(vec2(center_x, center_y - center_size));
        vertices.push_back(vec2(center_x, center_y - center_size - line_size));
        vertices.push_back(vec2(center_x + center_size, center_y));
        vertices.push_back(vec2(center_x + center_size + line_size, center_y));
        vertices.push_back(vec2(center_x - center_size, center_y));
        vertices.push_back(vec2(center_x - center_size - line_size, center_y));

        return vertices;
    }

    vector<vec2> generate_quad_vertices()
    {
        vector<vec2> vertices;
        vertices.push_back(vec2(-1.0f, 1.0f));
        vertices.push_back(vec2(1.0f, 1.0f));
        vertices.push_back(vec2(1.0f, -1.0f));
        vertices.push_back(vec2(1.0f, -1.0f));
        vertices.push_back(vec2(-1.0f, 1.0f));
        vertices.push_back(vec2(-1.0f, -1.0f));
        return vertices;
    }


    void render_pause_menu()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mat4 model = mat4(1.f);
        mat4 view  = mat4(1.f);
        mat4 proj  = mat4(1.f);
        //proj       = ortho(0.f, (float)context.screen_width, 0.f, (float)context.screen_height, -1.0f, 1.0f);

        this->shader->use();
        this->shader->set_mat4("model", model);
        this->shader->set_mat4("view", view);
        this->shader->set_mat4("projection", proj);
        this->shader->set_vec4("in_color", vec4(1.f, 1.f, 1.f, 0.5f));

        glBindVertexArray(this->pause_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
    }
};
