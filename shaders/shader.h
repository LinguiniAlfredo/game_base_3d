#pragma once

struct Shader {
    Shader(const char *vertex_path, const char *fragment_path);
    ~Shader();

    unsigned int id;
    
    void use();
};
