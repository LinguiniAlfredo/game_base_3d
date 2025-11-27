#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

out vec3 frag_pos;
out vec3 normal;

out VS_OUT {
    vec3 frag_pos;
    vec4 frag_pos_light_space;
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space_matrix;

void main()
{
    vs_out.frag_pos = vec3(model * vec4(a_pos, 1.0));
    vs_out.normal   = transpose(inverse(mat3(model))) * a_normal;
    vs_out.frag_pos_light_space = light_space_matrix * vec4(vs_out.frag_pos, 1.0);

    gl_Position = projection * view * model * vec4(a_pos, 1.0);
}
