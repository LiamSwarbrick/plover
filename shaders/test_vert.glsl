#version 450 core

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec2 vertex_texcoord;
layout (location = 2) in vec3 vertex_normal;

layout (location = 10) out vec2 tex_coord;

layout (location = 20) uniform mat4 mvp;

void
main(void)
{
    gl_Position = mvp * vec4(vertex_pos, 1.0);
    tex_coord = vertex_texcoord;
}
