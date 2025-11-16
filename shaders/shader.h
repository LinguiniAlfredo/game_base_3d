#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <cstdio>

using namespace std;
using namespace glm;

struct Shader {
    Shader(const char *vertex_path, const char *fragment_path);
    ~Shader();

    unsigned int id;
    
    void use();
    void set_int (const string &name, int  value) const;
    void set_mat4(const string &name, mat4 value) const;
};
