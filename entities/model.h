#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "mesh.h"

#include <cstdio>
#include <vector>
#include <iostream>
using namespace glm;

struct Model {
    vector<Mesh>    meshes;

    explicit Model(const char *path)
    {
        loadModel(path);
    }

    ~Model()
    {
        for (Mesh &mesh : meshes) {
            glDeleteVertexArrays(1, &mesh.VAO);
            glDeleteBuffers(1, &mesh.VBO);
            glDeleteBuffers(1, &mesh.EBO);
        }
    }

    void draw(Shader *shader, const vec3 position, const quat orientation, const vec3 scale)
    {
        for (Mesh &mesh : this->meshes) {
            mesh.draw(shader, position, orientation, scale);
        }
    }

private:
    void loadModel(const char *path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, 
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

    static Mesh process_mesh(const aiMesh *mesh, const aiScene *scene)
    {
        vector<Vertex>       vertices;
        vector<unsigned int> indices;

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
            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return Mesh(vertices, indices);
    }
};
