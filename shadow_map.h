#pragma once
#include "utils/shader.h"
#include <glm/glm.hpp>
#include <vector>
using namespace glm;

const unsigned int DEPTH_MAP_WIDTH = 1024;
const unsigned int DEPTH_MAP_HEIGHT = 1024;

struct ShadowMap
{
    unsigned int depth_map, FBO;
    Shader       *shader, *depth_quad_shader;
    mat4         light_space_matrix;

    ShadowMap();
    ~ShadowMap();

    void do_pass();

private:
    void init();
    void render_shadow_map();
    void render_depth_quad(float near_plane, float far_plane);
};
