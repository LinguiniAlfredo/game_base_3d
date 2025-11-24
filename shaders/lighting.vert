#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_pos;
out vec3 normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    frag_pos = vec3(model * vec4(aPos, 1.0));
    
    // rotate normals to match model matrix
    // inverse transpose is magic to keep normals on surface
    mat3 normal_matrix = inverse(transpose(mat3(model)));
    normal = normalize(normal_matrix * aNormal);
}
