#include "plvr_win32_platform.h"
#include "plvr_gl.h"
#include <stdio.h>
#include <stdlib.h>

void resize_callback(u32 width, u32 height)
{
    glViewport(0, 0, width, height);
}

#if 1
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int
main(int argc, char** argv)
#endif
{
    // NOTE: Setup window
    HWND window = win32_create_window(1280, 720, "Niiice", (Plvr_Resize_Callback_Proc)resize_callback);
    HDC device_context = GetDC(window);
    
    if (!win32_opengl_make_context(device_context))  // NOTE: OpenGL 4.5
    {
        printf("[In main()] win32_opengl_make_context() failed\n");
        return 1;
    }
    SetWindowTextA(window, (LPCSTR)glGetString(GL_VERSION));

    // NOTE: Initialization
    u32 shaders[2] = { compile_shader_file("shaders/test_vert.glsl", GL_VERTEX_SHADER),
                       compile_shader_file("shaders/test_frag.glsl", GL_FRAGMENT_SHADER) };
    u32 program = compile_shader_program(shaders, 2, "TextureCopy");

    u32 vao, vbo;
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    
    float vertices[] = {
    //  <------vec4 position------>  <-----vec4 color----->
        -1.0f, -1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  // bl red
         0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f,  // tl green
         1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f   // tr blue
    };
    glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_MAP_WRITE_BIT);

    // position (vec4)
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
    // color (vec4)
    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float));
    // link all together
    glVertexArrayVertexBuffer(vao, 0, vbo, (intptr_t)0, 8 * sizeof(float));
    
    Vector4 color_filter = { 1.0f, 1.0f, 1.0f, 1.0f };

    // NOTE: Main loop
    s64 last_time;
    s64 current_time = get_time();
    unused double delta;
    for (;;)
    {
        win32_poll_messages(window);
        
        // NOTE: Update
        last_time = current_time;
        current_time = get_time();
        delta = (double)(current_time - last_time) / 1000.0;

        if (key_is_down(VK_ESCAPE))
        {
            break;
        }
        
        static int dir = 1;
        if (color_filter.x <= 0.0f)
        {
            dir = 1;
        }
        else if (color_filter.x >= 1.0f)
        {
            dir = -1;
        }
        
        color_filter.x += 0.005 * dir;

        // NOTE: Render
        glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glProgramUniform4f(program, glGetUniformLocation(program, "color_filter"), color_filter.x, color_filter.y, color_filter.z, color_filter.w);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SwapBuffers(device_context);
    }

    return 0;
}
