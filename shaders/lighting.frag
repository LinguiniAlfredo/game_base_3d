#version 330 core

in VS_OUT {
    vec3 frag_pos;
    vec4 frag_pos_light_space;
    vec3 normal;
} fs_in;

out vec4 frag_color;

uniform sampler2D shadow_map;

uniform vec3 camera_pos;
uniform vec3 light_pos;
uniform vec3 light_color; // set light color here instead ?
uniform vec3 mesh_color;

float shadow_calculation(vec4 frag_pos_light_space)
{
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    float closest_depth = texture(shadow_map, proj_coords.xy).r;
    float current_depth = proj_coords.z;
    float shadow = current_depth > closest_depth ? 1.0 : 0.0;

   // vec3 normal = normalize(fs_in.normal);
   // vec3 light_dir = normalize(light_pos - fs_in.frag_pos);
   // float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);

   // float shadow = 0.0;
   // vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
   // for (int x = -1; x <= 1; ++x) {
   //     for (int y = -1; y <= 1; ++y) {
   //         float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
   //         shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
   //     }
   // }
   // shadow /= 9.0;

   // if (proj_coords.z > 1.0)
   //     shadow = 0.0;

    return shadow;
}

void main()
{
    vec3 color  = mesh_color;
    vec3 normal = normalize(fs_in.normal);
    
    vec3 ambient = 0.3 * light_color;

    vec3  light_dir = normalize(light_pos - fs_in.frag_pos);
    float diff      = max(dot(light_dir, normal), 0.0);
    vec3  diffuse   = diff * light_color;

    vec3 view_dir    = normalize(camera_pos - fs_in.frag_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);

    float spec = 0.0;
    vec3 halfway_dir = normalize(light_dir + view_dir);
    spec = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
    vec3 specular = spec * light_color;

    float shadow = shadow_calculation(fs_in.frag_pos_light_space);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    frag_color = vec4(lighting, 1.0);
}
