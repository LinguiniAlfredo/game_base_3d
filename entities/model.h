#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "mesh.h"

#include <cstdio>
#include <vector>
#include <string>
using namespace glm;
using namespace std;

struct Model
{
    vector<Mesh> meshes;
    vector<Texture> textures_loaded;
    const char *model_path, *texture_path;

    Model(const char *model_path, const char *texture_path = "")
    {
        this->model_path   = model_path;
        this->texture_path = texture_path;
        load_model();
    }

    ~Model()
    {
        for (Mesh &mesh : meshes) {
            glDeleteVertexArrays(1, &mesh.VAO);
            glDeleteBuffers(1, &mesh.VBO);
            glDeleteBuffers(1, &mesh.EBO);
        }
    }

    void render_shadow_map(Shader *shadow_map_shader, const vec3 position, const quat orientation, const vec3 scale)
    {
        for (Mesh &mesh : this->meshes) {
            mesh.render_shadow_map(shadow_map_shader, position, orientation, scale);
        }
    }

    void render(Shader *shader, const vec3 position, const quat orientation, const vec3 scale)
    {
        for (Mesh &mesh : this->meshes) {
            mesh.render(shader, position, orientation, scale);
        }
    }

private:
    void load_model()
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(this->model_path, 
                                                 aiProcess_Triangulate | 
                                                 aiProcess_GenSmoothNormals | 
                                                 aiProcess_FlipUVs | 
                                                 aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            printf("error loading model");
            return;
        }

        process_node(scene->mRootNode, scene);
    }

    void process_node(const aiNode *node, const aiScene *scene)
    {
        if (!node) {
            printf("found null node...skipping\n");
            return;
        }
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            if (node->mMeshes[i] < scene->mNumMeshes) {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                if (!mesh) {
                    printf("found null mesh...skipping\n");
                    return;
                }
                this->meshes.push_back(process_mesh(mesh, scene));
            } else {
                printf("invalid mesh id...\n");
            }
        }
        if (node->mNumChildren == 0) {
            return;
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            if (node->mChildren[i]) {
                process_node(node->mChildren[i], scene);
            } else {
                printf("null child found...skipping\n");
            }
        }
    }

    Mesh process_mesh(const aiMesh *mesh, const aiScene *scene)
    {
        vector<Vertex>       vertices;
        vector<unsigned int> indices;
        vector<Texture>      textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};
            vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }

            if (mesh->mTextureCoords[0]) {
                vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.tex_coords = vec;
            } else {
                vertex.tex_coords = vec2(0.f, 0.f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        //aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        //vector<Texture> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        //textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        if (texture_path != nullptr && *texture_path == '\0') {
            Texture texture;
            texture.id = texture_from_file(this->texture_path);
            texture.type = "";
            texture.path = this->texture_path;
            textures.push_back(texture);
        }

        return Mesh(vertices, indices, textures);
    }

    vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, string type_name)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                if (strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                Texture texture;
                texture.id = texture_from_file(str.C_Str());
                texture.type = type_name;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }

        return textures;
    }

    static unsigned int texture_from_file(const char *texture_path)
    {
        unsigned int texture_id;
        glGenTextures(1, &texture_id);

        int width, height ,nr_components;
        unsigned char *data = stbi_load(texture_path, &width, &height, &nr_components, 0);
        if (data) {
            GLenum format;
            if (nr_components == 1) {
                format = GL_RED;
            } else if (nr_components == 3) {
                format = GL_RGB;
            } else if (nr_components == 4) {
                format = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            printf("error loading texture...");
            stbi_image_free(data);
        }
        return texture_id;
    }
};

