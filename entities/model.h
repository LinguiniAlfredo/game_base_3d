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

    Model(const char *path)
    {
        loadModel(path);
    }

    void draw(Shader *shader, vec3 position)
    {
        for (unsigned int i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].draw(shader, position);
        }
    }

private:
    void loadModel(string const &path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            printf("error loading model");
            return;
        }

        process_node(scene->mRootNode, scene);
    }

    void process_node(aiNode *node, const aiScene *scene)
    {
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(process_mesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            process_node(node->mChildren[i], scene);
        }
    }

    Mesh process_mesh(aiMesh *mesh, const aiScene *scene)
    {
        vector<Vertex>       vertices;
        vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
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
