#pragma once
#include "../utils/file.h"

typedef struct Shader {
    unsigned int id;
} Shader;

void shader_create(Shader *shader, const char *vertex_path, const char *fragment_path)
{
    const char *vertex_code   = file_read(vertex_path);
    const char *fragment_code = file_read(fragment_path);

    unsigned int v, f;
    v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, (const char **)&vertex_code, NULL);
    glCompileShader(v);

    f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, (const char **)&fragment_code, NULL);
    glCompileShader(f);

    shader->id = glCreateProgram();
    glAttachShader(shader->id, v);
    glAttachShader(shader->id, f);
    glLinkProgram(shader->id);

    int success;
    char infoLog[512];
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(v, 512, NULL, infoLog);
        printf("vertex shader compilation error\n%s\n", infoLog);
    }

    // Check fragment shader compilation
    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(f, 512, NULL, infoLog);
        printf("fragment shader compilation error\n%s\n", infoLog);
    }

    // Check shader program linking
    glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader->id, 512, NULL, infoLog);
        printf("shader program linking error\n%s\n", infoLog);
    }

    glDeleteShader(v);
    glDeleteShader(f);
    free((void *)vertex_code);
    free((void *)fragment_code);
}

void shader_use(Shader *shader)
{
    glUseProgram(shader->id);
}

void shader_destroy(Shader *shader)
{
    glDeleteProgram(shader->id);
}
