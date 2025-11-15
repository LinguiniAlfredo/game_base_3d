#pragma once
#include "../shaders/shader.h"

typedef struct Rectangle {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    unsigned int indices[6];
    float        vertices[12];
    Shader       shader;
} Rectangle;

void rectangle_destroy(Rectangle *rectangle)
{
    glDeleteVertexArrays(1, &rectangle->VAO);
    glDeleteBuffers(1, &rectangle->VBO);
    glDeleteBuffers(1, &rectangle->EBO);
    shader_destroy(&rectangle->shader);
}

void rectangle_draw(Rectangle *rectangle)
{
    shader_use(&rectangle->shader);
    glBindVertexArray(rectangle->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void generateArrays(Rectangle *rectangle)
{
    glGenVertexArrays(1, &rectangle->VAO);
    glGenBuffers(1, &rectangle->VBO);
    glGenBuffers(1, &rectangle->EBO);

    glBindVertexArray(rectangle->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, rectangle->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, rectangle->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangle->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 6, rectangle->indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void rectangle_create(Rectangle *rectangle)
{
    rectangle->vertices[0]  =  0.5f;
    rectangle->vertices[1]  =  0.5f;
    rectangle->vertices[2]  =  0.0f;
    
    rectangle->vertices[3]  =  0.5f;
    rectangle->vertices[4]  = -0.5f;
    rectangle->vertices[5]  =  0.0f;
    
    rectangle->vertices[6]  = -0.5f;
    rectangle->vertices[7]  = -0.5f;
    rectangle->vertices[8]  =  0.0f;

    rectangle->vertices[9]  = -0.5f;
    rectangle->vertices[10] =  0.5f;
    rectangle->vertices[11] =  0.0f;

    rectangle->indices[0]   =  0;
    rectangle->indices[1]   =  1;
    rectangle->indices[2]   =  3;

    rectangle->indices[3]   =  1;
    rectangle->indices[4]   =  2;
    rectangle->indices[5]   =  3;

    shader_create(&rectangle->shader, "shaders/rectangle.vert", "shaders/rectangle.frag");
    generateArrays(rectangle);
}
