#pragma once
#include "../shaders/shader.h"
#include <array>
using namespace std;

struct Rectangle {
    Rectangle();
    ~Rectangle();

    unsigned int     VBO;
    unsigned int     VAO;
    unsigned int     EBO;
    array<int, 6>    indices;
    array<float, 24> vertices;
    Shader           *shader;

    void draw();

private:
    void generate_arrays();
};



