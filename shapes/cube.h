#pragma once
#include "../shaders/shader.h"
#include <array>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
using namespace std;

struct Cube {
    Cube(const vec3 position);
    ~Cube();

    unsigned int      VBO;
    unsigned int      VAO;
    array<float, 180> vertices;
    Shader            *shader;
    unsigned int      texture1, texture2;
    vec3              position;

    void draw();

private:
    void generate_arrays();
    void generate_texture();
};



