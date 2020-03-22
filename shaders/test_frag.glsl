#version 450 core

in vec2 tex_coord;
out vec4 frag_col;

layout (binding = 0) uniform sampler2D diffuse_map;
layout (binding = 1) uniform sampler2D specular_map;
layout (binding = 2) uniform sampler2D normal_map;
uniform vec4 color_filter;

void
main(void)
{
    frag_col = texture(diffuse_map, tex_coord) * color_filter;
}
