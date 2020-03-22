#ifndef PLVR_MESH_H
#define PLVR_MESH_H

#include "plvr_basic.h"
#include "plvr_gl.h"
#include "plvr_math.h"
#include <stdio.h>

typedef struct Vertex
{
    Vector3 position;
    Vector2 texcoord;
    Vector3 normal;
}
Vertex;

typedef struct Mesh
{
    u32 vao, vbo, ebo;
    u32 vertex_count, index_count;
    Vertex* vertices;
    u32* indices;
    Texture2D diffuse_map, specular_map, normal_map;
}
Mesh;

typedef struct Model
{
    char* filepath;
    u32 mesh_count;
    Mesh* meshes;
}
Model;

Mesh
mesh_create(Vertex* vertices, u32 vertex_count, u32* indices, u32 index_count, Texture2D diffuse_map, Texture2D specular_map, Texture2D normal_map)
{
    u32 vao, vbo, ebo;
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);

    glNamedBufferStorage(vbo, sizeof(Vertex) * vertex_count, vertices, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(ebo, sizeof(u32) * index_count, indices, GL_MAP_WRITE_BIT);

    // NOTE: Vertex position (Vector3) (location = 0)
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // NOTE: Vertex texture coords (Vector2) (location = 1)
    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texcoord));
    
    // NOTE: Vertex normal (Vector3) (location = 2)
    glVertexArrayAttribBinding(vao, 2, 0);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    
    // NOTE: Link all together
    glVertexArrayVertexBuffer(vao, 0, vbo, (GLintptr)0, sizeof(Vertex));

    Mesh mesh = { vao, vbo, ebo, vertex_count, index_count, vertices, indices, diffuse_map, specular_map, normal_map };
    
    return mesh;
}

void
mesh_render(Mesh mesh, Matrix4 model, Matrix4 camera_matrix, Vector4 diffuse_color, u32 shader)
{
    glUseProgram(shader);
    glProgramUniform4f(shader, glGetUniformLocation(shader, "color_filter"), diffuse_color.x, diffuse_color.y, diffuse_color.z, diffuse_color.w);
    glBindVertexArray(mesh.vao);
    glBindTextureUnit(0, mesh.diffuse_map.id);
    glBindTextureUnit(1, mesh.specular_map.id);
    glBindTextureUnit(2, mesh.normal_map.id);
    
    Matrix4 mvp;
    m4_mul(camera_matrix, model, mvp);
    glProgramUniformMatrix4fv(shader, 20, 1, GL_FALSE, (float*)mvp);
    glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, mesh.indices);
}

#endif  // PLVR_MESH_H