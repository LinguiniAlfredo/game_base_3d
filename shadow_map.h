#pragma once
#include "entities/mesh.h"
#include "utils/shader.h"
#include "context.h"
#include "utils/camera.h"
#include "shapes/lightcube.h"
#include "entities/entity.h"
#include "entities/floor.h"
#include <glm/glm.hpp>
#include <vector>
using namespace glm;

#define DEPTH_MAP_WIDTH  1024
#define DEPTH_MAP_HEIGHT 1024

struct ShadowMap
{
    unsigned int depth_map, FBO;
    Shader       *shader;
    mat4         light_space_matrix;

    ShadowMap()
    {
        shader = new Shader("shaders/depth.vert", "shaders/depth.frag");
        init();
    }

    ~ShadowMap()
    {
        delete shader;
    }

    void do_pass()
    {
        mat4 mat_proj, mat_view;

        mat_proj = ortho(-10.f, 10.f, -10.f, 10.f, context.camera->frustrum.near, context.camera->frustrum.far);
        mat_view = lookAt(context.light_cube->position, vec3(0.f), vec3(0.f, 1.f, 0.f));
        this->light_space_matrix = mat_proj * mat_view; // do this in shader ??

        shader->use();
        shader->set_mat4("light_space_matrix", this->light_space_matrix);

        glViewport(0, 0, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        render_scene();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, context.screen_width, context.screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

private:
    void init()
    {
        glGenFramebuffers(1, &this->FBO);
        glGenTextures(1, &this->depth_map);
        glBindTexture(GL_TEXTURE_2D, this->depth_map);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float border_color[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
        
        // attach depth texture as FBO's depth glBindBuffer
        glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth_map, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void render_scene()
    {
        context.light_cube->render_shadow_map(this->shader);
        for(unsigned int i = 0; i < context.entities.size(); i++) {
            context.entities[i]->render_shadow_map(this->shader);
        }
        context.floor->render_shadow_map(this->shader);
    }
};
