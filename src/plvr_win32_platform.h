#ifndef PLVR_WIN32_PLATFORM_H
#define PLVR_WIN32_PLATFORM_H

// NOTE: Include order is specific
#include "plvr_basic.h"
#include "plvr_system.h"
#include <windows.h>
#include <xinput.h>
#include <GL/gl3w.h>
#include <gl/wglext.h>
#include <stdio.h>

static int is_xinput_supported = 0;
static Gamepad_State gamepad_state;
static u8 previous_keyboard_state[512];
static u8 current_keyboard_state[512];
static int alt_is_down;
static u64 pc_frequency;  // NOTE: Performance counter frequency
static HGLRC win32_opengl_context;

// NOTE: Window resize callback
typedef void (*Plvr_Resize_Callback_Proc)(u32 width, u32 height);
Plvr_Resize_Callback_Proc win32_resize_callback;

/* OpenGL context */

static int
win32_opengl_make_context(HDC device_context)
{
    // NOTE: These WGL functions are needed to setup our OpenGL context
    PFNWGLCHOOSEPIXELFORMATARBPROC          wglChoosePixelFormatARB = NULL;
    PFNWGLCREATECONTEXTATTRIBSARBPROC       wglCreateContextAttribsARB = NULL;
    unused PFNWGLMAKECONTEXTCURRENTARBPROC  wglMakeContextCurrentARB = NULL;
    PFNWGLSWAPINTERVALEXTPROC               wglSwapIntervalEXT = NULL;

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize       = sizeof(pfd);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = 32;
    pfd.cAlphaBits  = 8;
    pfd.cDepthBits  = 24;

    int pixel_format = ChoosePixelFormat(device_context, &pfd);
    if (pixel_format == 0)
    {
		printf("[In win32_opengl_make_context()] pixel_format == 0 after ChoosePixelFormat()\n");
        return 0;
    }

    SetPixelFormat(device_context, pixel_format, &pfd);
    HGLRC test_render_context = wglCreateContext(device_context);
    wglMakeCurrent(device_context, test_render_context);

    // NOTE: Load the WGL ARB procs
    wglChoosePixelFormatARB     = (PFNWGLCHOOSEPIXELFORMATARBPROC)   wglGetProcAddress("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB  = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    wglMakeContextCurrentARB    = (PFNWGLMAKECONTEXTCURRENTARBPROC)  wglGetProcAddress("wglMakeContextCurrentARB");
    wglSwapIntervalEXT          = (PFNWGLSWAPINTERVALEXTPROC)        wglGetProcAddress("wglSwapIntervalEXT");

    // NOTE: Real pixel format
    // Now we can request multisampling
    const int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
    };
    
    UINT num_formats = 0;
    wglChoosePixelFormatARB(device_context, pixel_format_attribs, NULL, 1, &pixel_format, &num_formats);
    if (pixel_format == 0)
    {
        return 0;
    }

    const int context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        0
    };

    win32_opengl_context = wglCreateContextAttribsARB(device_context, test_render_context, context_attribs);
    if (win32_opengl_context == NULL)
    {
        return 0;
    }

    // NOTE: Delete test render context
    wglMakeCurrent(device_context, 0);
    wglDeleteContext(test_render_context);
    wglMakeCurrent(device_context, win32_opengl_context);
    wglSwapIntervalEXT(0);  // NOTE: 1 to enable vsync, 0 for to disable vsync
    
    // NOTE: Load OpenGL core profile procs
    //       This just saves loads of busy work with OpenGL proc loading
    if (gl3wInit())
    {
        printf("[In win32_opengl_make_context()] gl3wInit() failed\n");
        return 0;
    }

    return 1;
}


/* XInput */

// NOTE: XInput functions are dynamically loaded
#define XINPUT_GET_STATE(func_name) DWORD func_name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef XINPUT_GET_STATE(xinput_get_state);
XINPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
static xinput_get_state* XInputGetState_DynamicallyLoaded = XInputGetStateStub;

#define XINPUT_SET_STATE(func_name) DWORD func_name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef XINPUT_SET_STATE(xinput_set_state);
XINPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
static xinput_set_state* XInputSetState_DynamicallyLoaded = XInputSetStateStub;

#define XInputGetState XInputGetState_DynamicallyLoaded
#define XInputSetState XInputSetState_DynamicallyLoaded

static void
win32_load_xinput(void)
{
    HMODULE xinput_library = LoadLibraryA("xinput1_4.dll");
    if (xinput_library == NULL)
    {
        // NOTE: Fallback to 1.3 instead if 1.4 doesn't exist
        xinput_library = LoadLibraryA("xinput1_3.dll");
    }
    
    if (xinput_library != NULL)
    {
        XInputGetState = (xinput_get_state*)GetProcAddress(xinput_library, "XInputGetState");
        XInputSetState = (xinput_set_state*)GetProcAddress(xinput_library, "XInputSetState");
        is_xinput_supported = 1;
    }
}

/* Windowing */

static LRESULT CALLBACK
win32_window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_CLOSE:
            {
                DestroyWindow(window);
            }
            break;

        case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            break;

        // NOTE: Window resize callback
        case WM_SIZE:
            {
                RECT window_rect;
                GetClientRect(window, &window_rect);
                if (win32_opengl_context != NULL && win32_resize_callback != NULL)
                {
                    win32_resize_callback(window_rect.right - window_rect.left, window_rect.bottom - window_rect.top);
                }
            }
            break;

        // NOTE: Mouse messages
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            {
                
            }
            break;

        // NOTE: Keyboard messages
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
            {
                u32 key_code = wparam;
                int is_down = !(lparam & (1 << 31));
                alt_is_down = (lparam & (1 << 29));

                if (is_down && key_code == VK_F4 && alt_is_down)
                {
                    PostQuitMessage(0);
                }
                
                current_keyboard_state[key_code] = is_down;
            }
            break;

        // NOTE: Text input message
        case WM_CHAR:
            {
                // NOTE: For text input
                unused char character = wparam;
                
            }
            break;

        default:
            result = DefWindowProcA(window, message, wparam, lparam);
    }

    return result;
}

HWND
win32_create_window(int width, int height, char* title, Plvr_Resize_Callback_Proc resize_callback_proc)
{   
    win32_resize_callback = resize_callback_proc;

    // NOTE: Get clock frequency once on initialization
    if (QueryPerformanceFrequency((LARGE_INTEGER*)&pc_frequency))
    {
        // NOTE: System has high-resolution performance counter
        //       For systems WinXP or above this never fail

    }
    else
    {

    }

    win32_load_xinput();

    // NOTE: Register window class
    WNDCLASSEXA window_class;
    ZeroMemory(&window_class, sizeof(window_class));
    window_class.cbSize = sizeof(window_class);
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = win32_window_callback;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = GetModuleHandleA(NULL);
    window_class.hIcon = NULL;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = NULL;
    window_class.lpszMenuName = NULL;
    window_class.lpszClassName = "Plover_Window_Class";
    window_class.hIconSm = NULL;

    if (!RegisterClassExA(&window_class))
    {
        MessageBoxA(NULL, "[Error]Failed to register window class", "Error", MB_OK | MB_ICONEXCLAMATION);
        ExitProcess(1);
    }
    
    DWORD window_style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    RECT window_rect = { 0, 0, width, height };
    AdjustWindowRect(&window_rect, window_style, FALSE);
    
    HWND window = CreateWindowExA(0, window_class.lpszClassName, title, window_style, CW_USEDEFAULT, CW_USEDEFAULT, window_rect.right, window_rect.bottom, 0, 0, window_class.hInstance, 0);
    if (window == NULL)
    {
        MessageBoxA(NULL, "[Error]Failed to create window", "Error", MB_OK | MB_ICONEXCLAMATION);
        ExitProcess(1);
    }
    
    return window;
}

void
win32_poll_messages(HWND window)
{
    // NOTE: Copy current keyboard array to previous    
    CopyMemory(previous_keyboard_state, current_keyboard_state, sizeof(current_keyboard_state));

    // NOTE: Message Loop
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT)
        {
            ExitProcess(message.wParam);
        }
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    // NOTE: Update XInput
    for (DWORD controller_index = 0; controller_index < XUSER_MAX_COUNT; ++controller_index)
    {
        XINPUT_STATE controller_state;
        if (XInputGetState(controller_index, &controller_state) == ERROR_SUCCESS)
        {
            // NOTE: controller is plugged in
            XINPUT_GAMEPAD* xinput_gp = &controller_state.Gamepad;
            
            // NOTE: Gamepad_State.buttons is the same order as XINPUT_GAMEPAD.wButtons.
            // Otherwise you would have to unpack xinput_gp->wButtons and repack them
            // into gamepad_state.buttons in a different order
            gamepad_state.buttons = xinput_gp->wButtons;
            gamepad_state.left_trigger = xinput_gp->bLeftTrigger;
            gamepad_state.right_trigger = xinput_gp->bRightTrigger;
            gamepad_state.lstick_x = xinput_gp->sThumbLX;
            gamepad_state.lstick_y = xinput_gp->sThumbLY;
            gamepad_state.rstick_x = xinput_gp->sThumbRX;
            gamepad_state.rstick_y = xinput_gp->sThumbRY;
        }
        else
        {
            // NOTE: controller unavailable at this index
            // TODO
        }
    }
}

void
win32_swap_buffers(HDC device_context, u64 start_timems, u64 end_timems, u64 ms_per_frame)
{
    if (end_timems - start_timems < ms_per_frame)
    {
        Sleep(ms_per_frame - (end_timems - start_timems));
    }
    SwapBuffers(device_context);
}

/* NOTE: Implementations of plvr_system.h */
u64
get_timems()
{
    u64 clock_ticks;
    QueryPerformanceCounter((LARGE_INTEGER*)&clock_ticks);
    
    return (1000 * clock_ticks) / pc_frequency;  // miliseconds
}

/* NOTE: Implementations of plvr_input.h functions */
int
key_is_down(u32 key)
{
    return current_keyboard_state[key];
}

int
key_is_up(u32 key)
{
    return previous_keyboard_state[key];
}

int
key_just_pressed(u32 key)
{
    return current_keyboard_state[key] && !previous_keyboard_state[key];
}

int
key_just_released(u32 key)
{
    return previous_keyboard_state[key] && !current_keyboard_state[key];
}

void
mouse_get_relative(int from_x, int from_y, int* x, int* y)
{
    POINT cursor_pos;
    GetCursorPos(&cursor_pos);
    
    *x = cursor_pos.x - from_x;
    *y = cursor_pos.y - from_y;
}

void
mouse_set_pos(int x, int y)
{
    SetCursorPos(x, y);
}

int
gamepad_is_supported(void)
{
    return is_xinput_supported;
}

Gamepad_State*
gamepad_get_state(void)
{
    return &gamepad_state;
}



#endif  // PLVR_WIN32_PLATFORM_H
