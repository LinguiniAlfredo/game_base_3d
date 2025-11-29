#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "mesh.h"

#include <vector>
#include <string>
using namespace glm;
using namespace std;

struct Model
{
    vector<Mesh>    meshes;
    vector<Texture> textures_loaded;
    const char      *model_path;
    string          texture_path;

    Model(const char *model_path, const string &texture_path = "");
    ~Model();

    void render_shadow_map(Shader *shadow_map_shader, vec3 position, quat orientation, vec3 scale);
    void render(Shader *shader, vec3 position, quat orientation, vec3 scale);

private:
    void load_model();
    void process_node(const aiNode *node, const aiScene *scene);
    Mesh process_mesh(const aiMesh *mesh, const aiScene *scene);
    vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, string type_name);
    unsigned int texture_from_file(const string &texture_path);
};

