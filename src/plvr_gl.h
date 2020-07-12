#ifndef PLVR_GL_H
#define PLVR_GL_H

#include "plvr_basic.h"
#include "plvr_math.h"
#include "plvr_win32_platform.h"
#include <stdio.h>
#include <stdlib.h>  // NOTE: calloc() for loading shader source in compile_shader_file()

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>  // NOTE: For loading image formats (possible TODO: create own image file readers)

/* OpenGL debug */

static void APIENTRY opengl_callback(
        unused GLenum source,
        unused GLenum type,
        unused GLuint id,
        GLenum severity,
        unused GLsizei length,
        const GLchar* message,
        unused const void* user_param)
{
    
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        printf("[OpenGL callback]"ANSI_BLUE"%s\n\n"ANSI_RESET, message);
        printf(ANSI_RED"OpenGL error severity was high and needs fixing!\n\n"ANSI_RESET);
        ExitProcess(0);
    }
}

/* Shaders */

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

/* Textures */

typedef struct Texture2D  // (Will this get padded to 16 bytes?)
{
    u32 id;             // NOTE: OpenGL texture id
    s32 width, height;  // NOTE: Texture dimensions on x and y
    s32 color_channel_count;
}
Texture2D;

Texture2D
load_texture(const char* filepath)
{
    Texture2D texture = { 0 };
    
    // NOTE: Uses stbi_image.h to load image
    stbi_set_flip_vertically_on_load(1);
    u32* data = (u32*)stbi_load(filepath, &texture.width, &texture.height, &texture.color_channel_count, 0);
    if (data != NULL)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &texture.id);
        glTextureStorage2D(texture.id, 1, GL_RGBA8, texture.width, texture.height);
        glTextureSubImage2D(texture.id, 0, 0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        glTextureParameteri(texture.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(texture.id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture.id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
        printf("[Error]load_texture(%s) failed:  Failed to load texture", filepath);
    }
    
    stbi_image_free(data);

    return texture;
}


#endif  // PLVR_GL_H
