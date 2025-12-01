#version 330 core

in vec2 tex_coords;
out vec4 frag_color;

uniform sampler2D scene;
uniform vec2 resolution;

void main()
{
    vec2 tex_offset = 1.0 / resolution;
    vec4 color = vec4(0.0);

    float kernel[9] = float[](
            1.0, 2.0, 1.0,
            2.0, 4.0, 2.0,
            1.0, 2.0, 1.0
    );
    float weighted_sum = 16.0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            color += texture(scene, tex_coords + vec2(i, j) * tex_offset) * kernel[(i+1) * 3 + (j+1)];
        }
    }

    frag_color = color / weighted_sum;
}
