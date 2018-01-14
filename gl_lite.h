/*
    DEMO REVISION 0.1 
    =================
    This has been stripped of any functions unecessary for demo purposes
    to shrink final binary size, and will only output error messages
    when DEBUG is defined for the preprocessor

    gl_lite.h - Single-header multi-platform OpenGL function loader

    ----------------------------------------------------------------------------
    USAGE
    ----------------------------------------------------------------------------
    1) Add the following lines in exactly one of your cpp files to compile the
       implementation:

           #define GL_LITE_IMPLEMENTATION
           #include "gl_lite.h"

    2) In all other files in which you want to use OpenGL functions, simply 
       include this header file as follows:

           #include "gl_lite.h"

    3) Call gl_lite_init() before using any OpenGL function and after you have a
       valid OpenGL context.

    ----------------------------------------------------------------------------
    LICENSE
    ----------------------------------------------------------------------------
    This software is in the public domain. Where that dedication is not
    recognized, you are granted a perpetual, irrevocable license to copy,
    distribute, and modify this file as you see fit.
*/
#ifndef GL_LITE_H
#define GL_LITE_H

#define NOMINMAX
#define GLDECL WINAPI

#define GL_ARRAY_BUFFER                   0x8892 // Acquired from:
#define GL_ARRAY_BUFFER_BINDING           0x8894 // https://www.opengl.org/registry/api/GL/glext.h
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FUNC_ADD                       0x8006
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_STATIC_DRAW                    0x88E4
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
#define GL_VERTEX_SHADER                  0x8B31
#define GL_INFO_LOG_LENGTH                0x8B84


typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#ifndef DEBUG
#define DEBUG_LIST
#else
#define DEBUG_LIST \
    /* ret, name, params */ \
    GLE(void,      GetShaderInfoLog,        GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    GLE(void,      GetProgramInfoLog,       GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) \
    GLE(void,      GetShaderiv,             GLuint shader, GLenum pname, GLint *params) \
    GLE(void,      GetProgramiv,            GLuint program, GLenum pname, GLint *params) \
    /* end */
#endif

#define PAPAYA_GL_LIST \
    /* ret, name, params */ \
    GLE(void,      AttachShader,            GLuint program, GLuint shader) \
    GLE(void,      BindBuffer,              GLenum target, GLuint buffer) \
    GLE(void,      BufferData,              GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) \
    GLE(void,      CompileShader,           GLuint shader) \
    GLE(GLuint,    CreateProgram,           void) \
    GLE(GLuint,    CreateShader,            GLenum type) \
    GLE(void,      EnableVertexAttribArray, GLuint index) \
    GLE(void,      GenBuffers,              GLsizei n, GLuint *buffers) \
    GLE(GLint,     GetAttribLocation,       GLuint program, const GLchar *name) \
    GLE(GLint,     GetUniformLocation,      GLuint program, const GLchar *name) \
    GLE(void,      Uniform1f,               GLint location, GLfloat v0) \
    GLE(void,      LinkProgram,             GLuint program) \
    GLE(void,      ShaderSource,            GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) \
    GLE(void,      UseProgram,              GLuint program) \
    GLE(void,      GenVertexArrays,         GLsizei n, GLuint *arrays) \
    GLE(void,      BindVertexArray,         GLuint array) \
    GLE(void,      VertexAttribPointer,     GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) \
    /* end */

#define GLE(ret, name, ...) typedef ret GLDECL name##proc(__VA_ARGS__); extern name##proc * gl##name;
PAPAYA_GL_LIST
DEBUG_LIST
#undef GLE

bool gl_lite_init();

#endif //GL_LITE_H

// =============================================================================

#ifdef GL_LITE_IMPLEMENTATION

#define GLE(ret, name, ...) name##proc * gl##name;
PAPAYA_GL_LIST
DEBUG_LIST
#undef GLE

bool gl_lite_init()
{
    HINSTANCE dll = LoadLibraryA("opengl32.dll");
    typedef PROC WINAPI wglGetProcAddressproc(LPCSTR lpszProc);

    #ifdef DEBUG
    if (!dll) {
        printf("opengl32.dll not found.\n");
        return false;
    }
    #endif

    wglGetProcAddressproc* wglGetProcAddress =
        (wglGetProcAddressproc*)GetProcAddress(dll, "wglGetProcAddress");

    #ifdef DEBUG

    #define GLE(ret, name, ...)                                                                    \
            gl##name = (name##proc *)wglGetProcAddress("gl" #name);                                \
            if (!gl##name) {                                                                       \
                printf("Function gl" #name " couldn't be loaded from opengl32.dll\n");             \
                return false;                                                                      \
            }
        PAPAYA_GL_LIST
        DEBUG_LIST
    #undef GLE

    #else

    #define GLE(ret, name, ...)                                                                    \
        gl##name = (name##proc *)wglGetProcAddress("gl" #name);                                    \
            if (!gl##name) {                                                                       \
                return false;                                                                      \
            }
        PAPAYA_GL_LIST
    #undef GLE

    #endif // DEBUG

    return true;
}

#endif //GL_LITE_IMPLEMENTATION
