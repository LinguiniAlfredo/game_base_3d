#pragma once
#include <SDL2/SDL.h>
#include <math.h>

typedef struct Triangle {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int vertex_shader;
    unsigned int fragment_shader;
    unsigned int shader_program;
    float        vertices[18];
} Triangle;

const char *vertex_shader_source   = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec3 aColor;\n"
                                     "out vec3 ourColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos, 1.0);\n"
                                     "   ourColor = aColor;\n"
                                     "}\n\0";

const char *fragment_shader_source = "#version 330 core\n"
                                     "in vec3 ourColor;\n"
                                     "out vec4 FragColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   FragColor = vec4(ourColor, 1.0);\n"
                                     "}\n\0";

void triangle_destroy(Triangle *triangle)
{
    glDeleteVertexArrays(1, &triangle->VAO);
    glDeleteBuffers(1, &triangle->VBO);
    glDeleteProgram(triangle->shader_program);
}

void triangle_draw(Triangle *triangle)
{
    glUseProgram(triangle->shader_program);
    glBindVertexArray(triangle->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void compileShaders(Triangle *triangle)
{
    triangle->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(triangle->vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(triangle->vertex_shader);

    triangle->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(triangle->fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(triangle->fragment_shader);

    triangle->shader_program = glCreateProgram();
    glAttachShader(triangle->shader_program, triangle->vertex_shader);
    glAttachShader(triangle->shader_program, triangle->fragment_shader);
    glLinkProgram(triangle->shader_program);

    int success;
    char infoLog[512];
    glGetShaderiv(triangle->vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(triangle->vertex_shader, 512, NULL, infoLog);
        printf("vertex shader compilation error\n%s\n", infoLog);
    }

    // Check fragment shader compilation
    glGetShaderiv(triangle->fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(triangle->fragment_shader, 512, NULL, infoLog);
        printf("fragment shader compilation error\n%s\n", infoLog);
    }

    // Check shader program linking
    glGetProgramiv(triangle->shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(triangle->shader_program, 512, NULL, infoLog);
        printf("shader program linking error\n%s\n", infoLog);
    }

    glDeleteShader(triangle->vertex_shader);
    glDeleteShader(triangle->fragment_shader);
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

    compileShaders(triangle);
    generateBuffers(triangle);
}
