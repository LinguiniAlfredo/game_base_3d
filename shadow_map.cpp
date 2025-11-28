#include "shadow_map.h"
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

ShadowMap::ShadowMap()
{
    this->near_plane = 1.f;
    this->far_plane  = 200.f;
    shader = new Shader("shaders/depth.vert", "shaders/depth.frag");
    depth_quad_shader = new Shader("shaders/depth_quad.vert", "shaders/depth_quad.frag");
    init();
}

ShadowMap::~ShadowMap()
{
    delete shader;
    delete depth_quad_shader;
}

void ShadowMap::do_pass()
{
    mat4 mat_proj, mat_view;

    mat_proj = ortho(-100.f, 100.f, -100.f, 100.f, this->near_plane, this->far_plane);
    mat_view = lookAt(context.light_cube->position, vec3(0.0f), vec3(0.f, 1.f, 0.f));
    this->light_space_matrix = mat_proj * mat_view;

    shader->use();
    shader->set_mat4("light_space_matrix", this->light_space_matrix);

    glViewport(0, 0, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // cull the front face of meshes during depth map rendering
    // prevents peter-panning
    glCullFace(GL_FRONT);

    render_shadow_map();

    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, context.screen_width, context.screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void ShadowMap::init()
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

void ShadowMap::render_shadow_map()
{
    for(unsigned int i = 0; i < context.entities.size(); i++) {
        context.entities[i]->render_shadow_map(this->shader);
    }
    context.floor->render_shadow_map(this->shader);
}

void ShadowMap::render_depth_quad()
{
    glViewport(0, 0, context.screen_width/4, context.screen_height/4);
    this->depth_quad_shader->use();
    this->depth_quad_shader->set_float("near_plane", this->near_plane);
    this->depth_quad_shader->set_float("far_plane", this->far_plane);
    this->depth_quad_shader->set_int("depth_map", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->depth_map);

    unsigned int quadVAO = 0;
    unsigned int quadVBO;

    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glViewport(0, 0, context.screen_width, context.screen_height);
}
