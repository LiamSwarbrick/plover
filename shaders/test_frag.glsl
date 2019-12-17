#version 450 core

in vec4 color;
out vec4 frag_col;

uniform vec4 color_filter;

void
main(void)
{
    frag_col = color * color_filter;
}
