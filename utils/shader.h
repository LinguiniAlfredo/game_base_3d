#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <cstdio>
#include "../utils/file.h"

using namespace std;
using namespace glm;

struct Shader {
    unsigned int id;
    
    Shader(const char *vertex_path, const char *fragment_path)
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

        this->id = glCreateProgram();
        glAttachShader(this->id, v);
        glAttachShader(this->id, f);
        glLinkProgram(this->id);

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
        glGetProgramiv(this->id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(this->id, 512, NULL, infoLog);
            printf("shader program linking error\n%s\n", infoLog);
        }

        glDeleteShader(v);
        glDeleteShader(f);
        free((void *)vertex_code);
        free((void *)fragment_code);
    }

    ~Shader()
    {
        glDeleteProgram(this->id);
    }

    void use()
    {
        glUseProgram(this->id);
    }

    void set_int(const string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
    }

    void set_mat4(const string &name, mat4 value) const
    {
        unsigned int location = glGetUniformLocation(this->id, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
    }
};
