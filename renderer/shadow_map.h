#pragma once
#include "../utils/shader.h"
#include <glm/glm.hpp>
#include <vector>
using namespace glm;

const unsigned int DEPTH_MAP_WIDTH = 8124;
const unsigned int DEPTH_MAP_HEIGHT = 8124;

struct ShadowMap
{
    unsigned int depth_map, FBO;
    Shader       *shader, *depth_quad_shader;
    mat4         light_space_matrix;
    float        near_plane, far_plane;

    ShadowMap();
    ~ShadowMap();

    void do_pass();

    void init();
    void render_shadow_map();
    void render_depth_quad();
};
