#include "plvr_win32_platform.h"
#include "plvr_gl.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>

int win_width = 1280;
int win_height = 720;

void
resize_callback(u32 width, u32 height)
{
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
}

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // NOTE: Setup window
    HWND window = win32_create_window(win_width, win_height, "Niiice", (Plvr_Resize_Callback_Proc)resize_callback);
    HDC device_context = GetDC(window);
    
    if (!win32_opengl_make_context(device_context))  // NOTE: OpenGL 4.5
    {
        printf("[In main()] win32_opengl_make_context() failed\n");
        return 1;
    }
    SetWindowTextA(window, (LPCSTR)glGetString(GL_VERSION));
    ShowCursor(0);

    // NOTE: Set the OpenGL debug callback to opengl_callback from plvr_gl.h
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, 1);
    
    glViewport(0, 0, win_width, win_height);
    
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    // NOTE: Load Assets
    u32 shaders[2] = { compile_shader_file("shaders/test_vert.glsl", GL_VERTEX_SHADER),
                       compile_shader_file("shaders/test_frag.glsl", GL_FRAGMENT_SHADER) };
    u32 program = compile_shader_program(shaders, 2, "Texture_Shader");

    Texture2D texture = load_texture("assets/waters.png");

    // NOTE: Initialize geometry
    u32 vao, vbo;
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);

    float vertices[] = {
         // front face
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // 0. front bl
         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // 1. front tl
          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // 2. front tr
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // 0. front bl
          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // 2. front tr
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // 3. front br
         // back face
          0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // 7. back br
          0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // 6. back tr
         -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // 5. back tl
          0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // 7. back br
         -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // 5. back tl
         -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // 4. back bl
         // left face
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // 4. back bl
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // 5. back tl
         -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // 1. front tl
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // 4. back bl
         -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // 1. front t
         -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // 0. front bl
         // right face
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // 3. front br
          0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // 2. front tr
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // 6. back tr
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // 3. front br
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // 6. back tr
          0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // 7. back br
         // top face
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  // 1. front tl
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // 5. back tl
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // 6. back tr
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  // 1. front tl
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // 6. back tr
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // 2. front tr
         // bottom face
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // 4. back bl
         -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  // 0. front bl
          0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // 3. front br
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // 4. back bl
          0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // 3. front br
          0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // 7. back br
    };
    glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_MAP_WRITE_BIT);

    // NOTE: Position (vec3)
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    
    // NOTE: Texture coords (vec2)
    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    
    // NOTE: Link all together
    glVertexArrayVertexBuffer(vao, 0, vbo, (GLintptr)0, 5 * sizeof(float));

    // NOTE: Player
    Player player = { 0 };

    // NOTE: Main loop
    s64 last_time_ms;  // NOTE: Milliseconds
    s64 current_time_ms = get_timems();  // NOTE: Milliseconds
    float delta;        // NOTE: Seconds
    double time = 0.0;  // NOTE: Seconds
    for (;;)
    {
        u64 start_timems = get_timems();
        win32_poll_messages(window);
        
        // NOTE: Update
        last_time_ms = current_time_ms;
        current_time_ms = get_timems();
        delta = (float)(current_time_ms - last_time_ms) / 1000.0f;
        time += delta;

        if (key_is_down(VK_ESCAPE))
        {
            break;
        }
        
        char fps[64];
        sprintf(fps, "fps:%f", 1.0f / (float)delta);
        SetWindowTextA(window, fps);

        if (key_just_pressed('E'))
            ShowCursor(1);
        else if (key_just_pressed('R'))
            ShowCursor(0);

        // NOTE: Camera movement
        player.aspect = win_width / win_height;
        player_update(&player, delta);
        
        // NOTE: Render
        glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glProgramUniform4f(program, glGetUniformLocation(program, "color_filter"), 1.0f, 1.0f, 1.0f, 1.0f);
        glBindVertexArray(vao);
        glBindTextureUnit(0, texture.id);
        for (int i = 0; i < 10; ++i)
        {
            Matrix4 model = MAT4_IDENTITY_INIT;
            m4_translate(model, (Vector3){ 2.0f * i, 0.2f * i, 0.0f }, model);
            m4_rotate(model, rad(1.0f), (Vector3){ 0.5f, 0.5f, 0.0f }, model);

            Matrix4 mvp;
            m4_mul(player.camera_matrix, model, mvp);

            glProgramUniformMatrix4fv(program, 20, 1, GL_FALSE, (float*)mvp);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        win32_swap_buffers(device_context, start_timems, get_timems(), 1000 / 60);
    }

    return 0;
}
