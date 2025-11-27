#pragma once
#include "utils/shader.h"
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

    ShadowMap();
    ~ShadowMap();

    void do_pass();

private:
    void init();
    void render_scene();
};
