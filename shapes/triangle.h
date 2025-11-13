#pragma once

typedef struct Triangle {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int vertex_shader;
    unsigned int fragment_shader;
    unsigned int shader_program;
    float        vertices[9];
} Triangle;

const char *vertex_shader_source   = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\n\0";

const char *fragment_shader_source = "#version 330 core\n"
                                     "out vec4 FragColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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

void triangle_create(Triangle *triangle)
{
    triangle->vertices[0] = -0.5f;
    triangle->vertices[1] = -0.5f;
    triangle->vertices[2] =  0.0f;
    
    triangle->vertices[3] =  0.5f;
    triangle->vertices[4] = -0.5f;
    triangle->vertices[5] =  0.0f;
    
    triangle->vertices[6] =  0.0f;
    triangle->vertices[7] =  0.5f;
    triangle->vertices[8] =  0.0f;

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
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    // Check fragment shader compilation
    glGetShaderiv(triangle->fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(triangle->fragment_shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    // Check shader program linking
    glGetProgramiv(triangle->shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(triangle->shader_program, 512, NULL, infoLog);
        printf("ERROR::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(triangle->vertex_shader);
    glDeleteShader(triangle->fragment_shader);

    glGenVertexArrays(1, &triangle->VAO);
    glGenBuffers(1, &triangle->VBO);

    glBindVertexArray(triangle->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangle->VBO);

    // cpp can use sizeof(triangle->vertices)
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, triangle->vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
