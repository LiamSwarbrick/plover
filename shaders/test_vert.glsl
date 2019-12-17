#version 450 core

layout (location = 0) in vec4 vertex_pos;
layout (location = 1) in vec4 vertex_col;

out vec4 color;

void
main(void)
{
    gl_Position = vertex_pos;
    color = vertex_col;
}
