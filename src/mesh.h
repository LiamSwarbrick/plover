#ifndef PLVR_MESH_H
#define PLVR_MESH_H

#include "plvr_basic.h"
#include "plvr_gl.h"
#include "plvr_math.h"
#include <stdio.h>

typedef struct Vertex
{
    Vector3 position;
    Vector2 uv;
    Vector3 normal;
}
Vertex;

typedef struct Mesh
{
    u32 vao, vbo, ebo;
    u32 v_count, indices_count;
    Vertex* vertices;
    u32* indices;
    Texture2D diffuse_map, specular_map, normal_map;
}
Mesh;

Mesh
mesh_create(Vertex* vertices, u32 v_count, u32* indices, u32 indices_count, Texture2D diffuse_map, Texture2D specular_map, Texture2D normal_map)
{
    u32 vao, vbo, ebo;
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);

    glNamedBufferStorage(vbo, sizeof(Vertex) * v_count, vertices, GL_MAP_WRITE_BIT);
    glNamedBufferStorage(ebo, sizeof(u32) * indices_count, indices, GL_MAP_WRITE_BIT);

    // NOTE: Vertex position (Vector3) (location = 0)
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // NOTE: Vertex texture coords (Vector2) (location = 1)
    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
    
    // NOTE: Vertex normal (Vector3) (location = 2)
    glVertexArrayAttribBinding(vao, 2, 0);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    
    // NOTE: Link all together
    glVertexArrayVertexBuffer(vao, 0, vbo, (GLintptr)0, sizeof(Vertex));

    Mesh mesh = { vao, vbo, ebo, v_count, indices_count, vertices, indices, diffuse_map, specular_map, normal_map };
    
    return mesh;
}

void mesh_delete(Mesh* mesh)
{
    glDeleteVertexArrays(1, mesh->vao);
    glDeleteBuffers(1, mesh->vbo);
    glDeleteBuffers(1, mesh->ebo);
}

void
mesh_render(Mesh mesh, Matrix4 model, Matrix4 camera_matrix, Vector4 diffuse_color, u32 shader)
{
    glUseProgram(shader);
    glProgramUniform4f(shader, glGetUniformLocation(shader, "color_filter"), diffuse_color.x, diffuse_color.y, diffuse_color.z, diffuse_color.w);
    glBindVertexArray(mesh.vao);
    if (mesh.diffuse_map.id != 0)
        glBindTextureUnit(0, mesh.diffuse_map.id);
    if (mesh.specular_map.id != 0)
        glBindTextureUnit(1, mesh.specular_map.id);
    if (mesh.normal_map.id != 0)
        glBindTextureUnit(2, mesh.normal_map.id);
    
    Matrix4 mvp;
    m4_mul(camera_matrix, model, mvp);
    glProgramUniformMatrix4fv(shader, 20, 1, GL_FALSE, (float*)mvp);
    glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, mesh.indices);
}



/* obj parser not working
int
load_obj(const char* filepath, Mesh* out_mesh)
{
    // TODO FUTURE: Write a faster parser,
    // REF:  http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
    u32* vertex_indices; 
    u32* uv_indices;
    u32* normal_indices;
    
    // NOTE: line headers:
    //        - v: vertex
    //        - vt: uv / texcoord
    //        - vn: vertex normal
    //        - f: face

    FILE* fp = fopen(filepath, "r");
    
    // NOTE: Find how many vertices, uvs, normals and faces
    int obj_v_count = 0;
    int obj_vt_count = 0;
    int obj_vn_count = 0;
    int obj_f_count = 0;
    
    char s[128];
    while (1)
    {
        if (fscanf(fp, "%s", s) == EOF)
        {
            break;
        }

        if (strcmp(s, "v") == 0)
        {
            ++obj_v_count;
        }
        else if (strcmp(s, "vt") == 0)
        {
            ++obj_vt_count;
        }
        else if (strcmp(s, "vn") == 0)
        {
            ++obj_vn_count;
        }
        else if (strcmp(s, "f") == 0)
        {
            ++obj_f_count;
        }
    }
    rewind(fp);

    u32 vertices_count = obj_f_count * 3;
    u32 indices_count = vertices_count;

    // NOTE: One allocation to reduce alloc overhead
    void* obj_data_memblock = malloc((obj_v_count * sizeof(Vector3*)) + (obj_vt_count * sizeof(Vector2)) + (obj_vn_count + sizeof(Vector3)));
    Vector3* positions = obj_data_memblock;
    Vector2* uvs = obj_data_memblock + (obj_v_count * sizeof(Vector3*));
    Vector3* normals = (void*)uvs + (obj_vt_count * sizeof(Vector2));

    Vertex* vertices = malloc(vertices_count * sizeof(Vertex)) + (indices_count * sizeof(u32));
    u32* indices = (void*)vertices + (indices_count * sizeof(u32));  // NOTE: Currently just 0,1,2,3,4,...
    
    while (1)
    {
        char line_header[128];

        if (fscanf(fp, "%s", line_header) == EOF)
        {
            break;
        }

        static int positions_added = 0;
        static int uvs_added = 0;
        static int normals_added = 0;
        static int faces_added = 0;
        static int vertices_added = 0;

        if (strcmp(line_header, "v") == 0)  // v: vertex position
        {
            Vector3 position;
            fscanf(fp, "%f %f %f\n", &position.x, &position.y, &position.z);
            positions[positions_added++] = position;
        }
        else if (strcmp(line_header, "vt") == 0)  // vt: uv
        {
            Vector2 uv;
            fscanf(fp, "%f %f\n", &uv.x, &uv.y);
            uvs[uvs_added++] = uv;
        }
        else if (strcmp(line_header, "vn") == 0)  // vn: normal
        {
            Vector3 normal;
            fscanf(fp, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            normals[normals_added++] = normal;
        }
        else if (strcmp(line_header, "f") == 0)  // f: face
        {
            u32 position_index[3];
            u32 uv_index[3];
            u32 normal_index[3];
            if (fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &position_index[0], &uv_index[0], &normal_index[0], &position_index[1], &uv_index[1], &normal_index[1], &position_index[2], &uv_index[2], &normal_index[2]) != 9)
            {
                printf("Parser too bad to read this file, fix parser or fix obj file!\n");
                return 0;  // failure
            }

            // NOTE: Create 3 vertices from index data and add to vertices array
            indices[vertices_added] = vertices_added;  // NOTE: Currently indices aren't optimially implemented
            vertices[vertices_added++] = (Vertex){ positions[position_index[0]], uvs[uv_index[0]], normals[normal_index[0]] };
            indices[vertices_added] = vertices_added;
            vertices[vertices_added++] = (Vertex){ positions[position_index[1]], uvs[uv_index[1]], normals[normal_index[1]] };
            indices[vertices_added] = vertices_added;
            vertices[vertices_added++] = (Vertex){ positions[position_index[2]], uvs[uv_index[2]], normals[normal_index[2]] };

            ++faces_added;
        }
    }
    
    Mesh mesh = mesh_create(vertices, vertices_count, indices, indices_count, (Texture2D){ 0 }, (Texture2D){ 0 }, (Texture2D){ 0 });
    out_mesh = &mesh;

    fclose(fp);
    free(obj_data_memblock);

    return 1;  // success
}
*/

#endif  // PLVR_MESH_H
