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

struct Rectangle {
    Rectangle();
    ~Rectangle();

    unsigned int     VBO;
    unsigned int     VAO;
    unsigned int     EBO;
    array<int, 6>    indices;
    array<float, 32> vertices;
    Shader           *shader;
    unsigned int     texture1, texture2;

    void draw();

private:
    void generate_arrays();
    void generate_texture();
};



