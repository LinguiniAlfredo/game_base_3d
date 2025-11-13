#pragma once

typedef struct Rectangle {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    unsigned int vertex_shader;
    unsigned int fragment_shader;
    unsigned int shader_program;
    unsigned int indices[6];
    float        vertices[12];
} Rectangle;

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

void rectangle_destroy(Rectangle *rectangle)
{
    glDeleteVertexArrays(1, &rectangle->VAO);
    glDeleteBuffers(1, &rectangle->VBO);
    glDeleteBuffers(1, &rectangle->EBO);
    glDeleteProgram(rectangle->shader_program);
}

void rectangle_draw(Rectangle *rectangle)
{
    glUseProgram(rectangle->shader_program);
    glBindVertexArray(rectangle->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void compileShaders(Rectangle *rectangle)
{
    rectangle->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(rectangle->vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(rectangle->vertex_shader);

    rectangle->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(rectangle->fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(rectangle->fragment_shader);

    rectangle->shader_program = glCreateProgram();
    glAttachShader(rectangle->shader_program, rectangle->vertex_shader);
    glAttachShader(rectangle->shader_program, rectangle->fragment_shader);
    glLinkProgram(rectangle->shader_program);

    int success;
    char infoLog[512];
    glGetShaderiv(rectangle->vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(rectangle->vertex_shader, 512, NULL, infoLog);
        printf("vertex shader compilation error\n%s\n", infoLog);
    }

    // Check fragment shader compilation
    glGetShaderiv(rectangle->fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(rectangle->fragment_shader, 512, NULL, infoLog);
        printf("fragment shader compilation error\n%s\n", infoLog);
    }

    // Check shader program linking
    glGetProgramiv(rectangle->shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(rectangle->shader_program, 512, NULL, infoLog);
        printf("shader program linking error\n%s\n", infoLog);
    }

    glDeleteShader(rectangle->vertex_shader);
    glDeleteShader(rectangle->fragment_shader);
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

    compileShaders(rectangle);
    generateArrays(rectangle);
}
