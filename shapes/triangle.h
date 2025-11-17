#pragma once
#include <SDL2/SDL.h>
#include <math.h>
#include "../utils/shader.h"
#include "../utils/arena.h"

typedef struct Triangle {
    unsigned int VBO;
    unsigned int VAO;
    float        vertices[18];
    Shader       shader;
} Triangle;

void triangle_destroy(Triangle *triangle)
{
    glDeleteVertexArrays(1, &triangle->VAO);
    glDeleteBuffers(1, &triangle->VBO);
    shader_destroy(&triangle->shader);
}

void triangle_draw(Triangle *triangle)
{
    shader_use(&triangle->shader);
    glBindVertexArray(triangle->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void generateBuffers(Triangle *triangle)
{
    glGenVertexArrays(1, &triangle->VAO);
    glGenBuffers(1, &triangle->VBO);

    glBindVertexArray(triangle->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangle->VBO);

    // cpp can use sizeof(triangle->vertices)
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, triangle->vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void triangle_create(Triangle *triangle)
{
    triangle->vertices[0]  =  0.5f; // position
    triangle->vertices[1]  = -0.5f;
    triangle->vertices[2]  =  0.0f;

    triangle->vertices[3]  =  1.0f; // color
    triangle->vertices[4]  =  0.0f;
    triangle->vertices[5]  =  0.0f;
    
    triangle->vertices[6]  = -0.5f; // position
    triangle->vertices[7]  = -0.5f;
    triangle->vertices[8]  =  0.0f;

    triangle->vertices[9]  =  0.0f; // color
    triangle->vertices[10] =  1.0f;
    triangle->vertices[11] =  0.0f;
    
    triangle->vertices[12] =  0.0f; // position
    triangle->vertices[13] =  0.5f;
    triangle->vertices[14] =  0.0f;

    triangle->vertices[15] =  0.0f; // color
    triangle->vertices[16] =  0.0f;
    triangle->vertices[17] =  1.0f;

    shader_create(&triangle->shader, "shaders/vertex.vert", "shaders/fragment.frag");
    generateBuffers(triangle);
}
