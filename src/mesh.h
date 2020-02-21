#ifndef PLVR_MESH_H
#define PLVR_MESH_H

#include "plvr_basic.h"
#include "plvr_gl.h"
#include "plvr_math.h"

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
    Texture2D texture;
}
Mesh;

Mesh
mesh_create(Vertex* vertices, u32 vertex_count, u32* indices, u32 index_count, Texture2D texture)
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

    Mesh mesh = { vao, vbo, ebo, vertex_count, index_count, vertices, indices, texture };
    
    return mesh;
}

void
mesh_render(Mesh mesh, Matrix4 model, Matrix4 camera_matrix, u32 shader)
{
    glUseProgram(shader);
    glProgramUniform4f(shader, glGetUniformLocation(shader, "color_filter"), 1.0f, 1.0f, 1.0f, 1.0f);
    glBindVertexArray(mesh.vao);
    glBindTextureUnit(0, mesh.texture.id);
    
    Matrix4 mvp;
    m4_mul(camera_matrix, model, mvp);
    glProgramUniformMatrix4fv(shader, 20, 1, GL_FALSE, (float*)mvp);
    glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, mesh.indices);
}

#endif  // PLVR_MESH_H