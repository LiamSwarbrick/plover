#version 450 core

layout (location = 0) in vec4 vertex_pos;
layout (location = 1) in vec2 vertex_texcoord;

out vec2 tex_coord;

layout (location = 20) uniform mat4 mvp;

void
main(void)
{
    gl_Position = mvp * vertex_pos;
    tex_coord = vertex_texcoord;
}
