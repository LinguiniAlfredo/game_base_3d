#pragma once
#include <string>
#include <vector>
#include <cstdio>
#include <glm/glm.hpp>
#include "../utils/shader.h"
using namespace std;
using namespace glm;

struct Skybox {

    unsigned int texture_id;
    unsigned int VAO, VBO;
    Shader       *shader;
    vector<vec3> vertices;

    Skybox()
    {
        // order of these needs to match GL_TEXTURE_CUBE_MAP target order
        // we flip front and back however because we are viewing from inside the cube
        vector<string> faces = {
            "resources/textures/skyboxes/right.jpg",
            "resources/textures/skyboxes/left.jpg",
            "resources/textures/skyboxes/top.jpg",
            "resources/textures/skyboxes/bottom.jpg",
            "resources/textures/skyboxes/front.jpg",
            "resources/textures/skyboxes/back.jpg"
        };
        this->texture_id = load_texture(faces);
        this->shader     = new Shader("shaders/skybox.vert", "shaders/skybox.frag");
        this->vertices   = get_vertices();
        init();
    }
    
    ~Skybox()
    {
        glDeleteVertexArrays(1, &this->VAO);
        glDeleteBuffers(1, &this->VBO);
        glDeleteTextures(1, &this->texture_id);
    }

    void draw()
    {
        glDepthFunc(GL_LEQUAL);
        this->shader->use();
        this->shader->set_int("skybox", 0); // not needed, i think it does this by default

        mat4 mat_view = mat4(1.0f);
        mat_view = context.camera->get_view_matrix();
        mat_view = mat4(mat3(mat_view)); // convert to mat3 to remove translation, then back to mat4, so skybox doesn't move

        mat4 mat_proj = mat4(1.0f);
        mat_proj = context.camera->get_perspective_matrix();

        shader->set_mat4("view", mat_view);
        shader->set_mat4("projection", mat_proj);

        glBindVertexArray(this->VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture_id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
    }

private:
    static unsigned int load_texture(vector<string> faces)
    {
        unsigned int texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            } else {
                printf("cubemap failed to load...\n");
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texture_id;
    }

    static vector<vec3> get_vertices()
    {
        vector<vec3> vector = {
            vec3(-1.0f,  1.0f, -1.0f),
            vec3(-1.0f, -1.0f, -1.0f),
            vec3( 1.0f, -1.0f, -1.0f),
            vec3( 1.0f, -1.0f, -1.0f),
            vec3( 1.0f,  1.0f, -1.0f),
            vec3(-1.0f,  1.0f, -1.0f),
            vec3(-1.0f, -1.0f,  1.0f),
            vec3(-1.0f, -1.0f, -1.0f),
            vec3(-1.0f,  1.0f, -1.0f),
            vec3(-1.0f,  1.0f, -1.0f),
            vec3(-1.0f,  1.0f,  1.0f),
            vec3(-1.0f, -1.0f,  1.0f),
            vec3( 1.0f, -1.0f, -1.0f),
            vec3( 1.0f, -1.0f,  1.0f),
            vec3( 1.0f,  1.0f,  1.0f),
            vec3( 1.0f,  1.0f,  1.0f),
            vec3( 1.0f,  1.0f, -1.0f),
            vec3( 1.0f, -1.0f, -1.0f),
            vec3(-1.0f, -1.0f,  1.0f),
            vec3(-1.0f,  1.0f,  1.0f),
            vec3( 1.0f,  1.0f,  1.0f),
            vec3( 1.0f,  1.0f,  1.0f),
            vec3( 1.0f, -1.0f,  1.0f),
            vec3(-1.0f, -1.0f,  1.0f),
            vec3(-1.0f,  1.0f, -1.0f),
            vec3( 1.0f,  1.0f, -1.0f),
            vec3( 1.0f,  1.0f,  1.0f),
            vec3( 1.0f,  1.0f,  1.0f),
            vec3(-1.0f,  1.0f,  1.0f),
            vec3(-1.0f,  1.0f, -1.0f),
            vec3(-1.0f, -1.0f, -1.0f),
            vec3(-1.0f, -1.0f,  1.0f),
            vec3( 1.0f, -1.0f, -1.0f),
            vec3( 1.0f, -1.0f, -1.0f),
            vec3(-1.0f, -1.0f,  1.0f),
            vec3( 1.0f, -1.0f,  1.0f)
        };
        return vector;
    }

    void init()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }
};
