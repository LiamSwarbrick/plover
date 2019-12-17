#ifndef PLVR_GL_H
#define PLVR_GL_H

#include "plvr_basic.h"
#include "plvr_math.h"
#include "plvr_win32_platform.h"
#include <stdio.h>
#include <stdlib.h>

/* NOTE: Types */


/* NOTE: OpenGL debug */

static void APIENTRY openglCallbackFunction(
        unused GLenum source,
        unused GLenum type,
        unused GLuint id,
        GLenum severity,
        unused GLsizei length,
        const GLchar* message,
        unused const void* user_param)
{
    printf("[OpenGL callback]%s\n", message);
    
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        printf("OpenGL error severity was high and needs fixing!\n");
    }
}

/* NOTE: Shaders */

u32
compile_shader_file(char* filepath, u32 shader_type)
{
    // NOTE: Read shader source file
    size_t file_size;
    char* source_buffer;
    {
        FILE* fp = fopen(filepath, "r");  

        fseek(fp, 0L, SEEK_END);
        file_size = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        source_buffer = calloc(file_size, 1);

        fread(source_buffer, 1, file_size, fp);

        fclose(fp);
    }
    
    // NOTE: Call shader compiler
    u32 shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, (const char * const*)&source_buffer, NULL);
    glCompileShader(shader);
    
    free(source_buffer);

    // NOTE: Check for failure
    int was_successful;
    char info_log[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &was_successful);
    if (!was_successful)
    {
        glGetShaderInfoLog(shader, 1024, NULL, info_log);
        printf("[ERROR]Shader compile error (%s):\n%s", filepath, info_log);
    }

    return shader;
}

u32
compile_shader_program(u32* shaders, int shader_count, const char* opengl_debug_name)
{
    // NOTE: Create shader program, attach shaders, then link shader program
    u32 program = glCreateProgram();
    glObjectLabel(GL_PROGRAM, program, -1, opengl_debug_name);

    for (int i = 0; i < shader_count; ++i)
    {
        glAttachShader(program, shaders[i]);
    }
    glLinkProgram(program);

    // NOTE: Check for failure
    int was_successful;
    char info_log[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &was_successful);
    if (!was_successful)
    {
        glGetProgramInfoLog(program, 1024, NULL, info_log);
        printf("[ERROR]Shader program link error:\n%s", info_log);
    }

    // NOTE: Detach then delete shaders
    for (int i = 0; i < shader_count; ++i)
    {
        glDetachShader(program, shaders[i]);
        glDeleteShader(shaders[i]);
    }

    return program;
}

/* NOTE: Textures */



#endif  // PLVR_GL_H