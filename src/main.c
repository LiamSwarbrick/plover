#include "plvr_win32_platform.h"
#include "plvr_gl.h"
#include "player.h"
#include "mesh.h"
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);  // NOTE: Premultiplied alpha

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // NOTE: Wireframe mode
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    
    // NOTE: Load Assets
    u32 shaders[2] = { compile_shader_file("shaders/test_vert.glsl", GL_VERTEX_SHADER),
                       compile_shader_file("shaders/test_frag.glsl", GL_FRAGMENT_SHADER) };
    u32 program = compile_shader_program(shaders, 2, "Texture_Shader");

    Texture2D diffuse_map = load_texture("assets/thingpng.png");//flower_blue_orchid.png");
    Texture2D normal_map = load_texture("assets/thingpng.png");

    // NOTE: Mesh
    // Mesh mesh = gen_primitive_cube(2.0f, 3.0f, 4.0f, diffuse_map, (Texture2D){ 0 }, normal_map);
    
    float width = 2.0f;
    float height = 3.0f;
    float depth = 4.0f;
    Vertex vertices[] = {
        // front face
        { { -width/2.0f, -height/2.0f,  depth/2.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } },  // 0. front bl
        { { -width/2.0f,  height/2.0f,  depth/2.0f }, { 0.0f, 1.0f }, {  0.0f,  0.0f,  1.0f } },  // 1. front tl
        { {  width/2.0f,  height/2.0f,  depth/2.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f,  1.0f } },  // 2. front tr
        { { -width/2.0f, -height/2.0f,  depth/2.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } },  // 0. front bl
        { {  width/2.0f,  height/2.0f,  depth/2.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f,  1.0f } },  // 2. front tr
        { {  width/2.0f, -height/2.0f,  depth/2.0f }, { 1.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } },  // 3. front br
        // back face
        { {  width/2.0f, -height/2.0f, -depth/2.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } },  // 7. back br
        { {  width/2.0f,  height/2.0f, -depth/2.0f }, { 0.0f, 1.0f }, {  0.0f,  0.0f, -1.0f } },  // 6. back tr
        { { -width/2.0f,  height/2.0f, -depth/2.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f, -1.0f } },  // 5. back tl
        { {  width/2.0f, -height/2.0f, -depth/2.0f }, { 0.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } },  // 7. back br
        { { -width/2.0f,  height/2.0f, -depth/2.0f }, { 1.0f, 1.0f }, {  0.0f,  0.0f, -1.0f } },  // 5. back tl
        { { -width/2.0f, -height/2.0f, -depth/2.0f }, { 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } },  // 4. back bl
        // left face
        { { -width/2.0f, -height/2.0f, -depth/2.0f }, { 0.0f, 0.0f }, { -1.0f,  0.0f,  0.0f } },  // 4. back bl
        { { -width/2.0f,  height/2.0f, -depth/2.0f }, { 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },  // 5. back tl
        { { -width/2.0f,  height/2.0f,  depth/2.0f }, { 1.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },  // 1. front tl
        { { -width/2.0f, -height/2.0f, -depth/2.0f }, { 0.0f, 0.0f }, { -1.0f,  0.0f,  0.0f } },  // 4. back bl
        { { -width/2.0f,  height/2.0f,  depth/2.0f }, { 1.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },  // 1. front t
        { { -width/2.0f, -height/2.0f,  depth/2.0f }, { 1.0f, 0.0f }, { -1.0f,  0.0f,  0.0f } },  // 0. front bl
        // right face
        { {  width/2.0f, -height/2.0f,  depth/2.0f }, { 0.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } },  // 3. front br
        { {  width/2.0f,  height/2.0f,  depth/2.0f }, { 0.0f, 1.0f }, {  1.0f,  0.0f,  0.0f } },  // 2. front tr
        { {  width/2.0f,  height/2.0f, -depth/2.0f }, { 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f } },  // 6. back tr
        { {  width/2.0f, -height/2.0f,  depth/2.0f }, { 0.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } },  // 3. front br
        { {  width/2.0f,  height/2.0f, -depth/2.0f }, { 1.0f, 1.0f }, {  1.0f,  0.0f,  0.0f } },  // 6. back tr
        { {  width/2.0f, -height/2.0f, -depth/2.0f }, { 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } },  // 7. back br
        // top face
        { { -width/2.0f,  height/2.0f,  depth/2.0f }, { 0.0f, 0.0f }, {  0.0f,  1.0f,  0.0f } },  // 1. front tl
        { { -width/2.0f,  height/2.0f, -depth/2.0f }, { 0.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },  // 5. back tl
        { {  width/2.0f,  height/2.0f, -depth/2.0f }, { 1.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },  // 6. back tr
        { { -width/2.0f,  height/2.0f,  depth/2.0f }, { 0.0f, 0.0f }, {  0.0f,  1.0f,  0.0f } },  // 1. front tl
        { {  width/2.0f,  height/2.0f, -depth/2.0f }, { 1.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },  // 6. back tr
        { {  width/2.0f,  height/2.0f,  depth/2.0f }, { 1.0f, 0.0f }, {  0.0f,  1.0f,  0.0f } },  // 2. front tr
        // bottom face
        { { -width/2.0f, -height/2.0f, -depth/2.0f }, { 0.0f, 0.0f }, {  0.0f, -1.0f,  0.0f } },  // 4. back bl
        { { -width/2.0f, -height/2.0f,  depth/2.0f }, { 0.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },  // 0. front bl
        { {  width/2.0f, -height/2.0f,  depth/2.0f }, { 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },  // 3. front br
        { { -width/2.0f, -height/2.0f, -depth/2.0f }, { 0.0f, 0.0f }, {  0.0f, -1.0f,  0.0f } },  // 4. back bl
        { {  width/2.0f, -height/2.0f,  depth/2.0f }, { 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },  // 3. front br
        { {  width/2.0f, -height/2.0f, -depth/2.0f }, { 1.0f, 0.0f }, {  0.0f, -1.0f,  0.0f } },  // 7. back br
    };
    u32 indices[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
    };

    Mesh mesh = mesh_create(vertices, 36, indices, 36, diffuse_map, (Texture2D){ 0 }, normal_map);
    
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
        player.aspect = (float)win_width / (float)win_height;
        player_update(&player, delta);
        
        // NOTE: Render
        glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Matrix4 model = MAT4_IDENTITY_INIT;
        mesh_render(mesh, model, player.camera_matrix, (Vector4){ 1.0f, 1.0f, 1.0f, 1.0f }, program);
        
        win32_swap_buffers(device_context, start_timems, get_timems(), 1000 / 100);
    }

    return 0;
}
