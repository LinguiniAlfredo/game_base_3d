#pragma once
#include "../utils/shader.h"
#include <array>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
using namespace std;

struct LightCube {
    LightCube(const vec3 position);
    ~LightCube();

    unsigned int      VBO;
    unsigned int      VAO;
    array<float, 108> vertices;
    Shader            *shader;
    vec3              position;

    void draw();

private:
    void generate_arrays();
};



