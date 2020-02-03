#version 450 core

in vec2 tex_coord;
out vec4 frag_col;

uniform sampler2D texture0;
uniform vec4 color_filter;

void
main(void)
{
    frag_col = texture(texture0, tex_coord) * color_filter;
}
