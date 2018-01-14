#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>

#else

extern "C" {
    void * __cdecl memset(void *, int, size_t);
    int _fltused;
}
#pragma intrinsic(memset)

#pragma function(memset)
void *memset(void *dest, int c, size_t count) {
    char *bytes = (char *)dest;
    while (count--) {
        *bytes++ = (char)c;
    }
    return dest;
}

#endif //DEBUG

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>

#include <GL/gl.h>

#define GL_LITE_IMPLEMENTATION
#include "gl_lite.h"

#ifdef DEBUG

#include <assert.h>
#define GL_CHECK(x) do { x; GLenum err = glGetError(); assert(err == GL_NO_ERROR); } while(0)
#define PRINT(...) do { printf(__VA_ARGS__); } while(0);

#else

#define GL_CHECK(x)
#define PRINT(...)

#endif //DEBUG

#ifdef DEBUG
void get_shader_err(GLuint shader) {
    GLint err_log_max_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &err_log_max_length);
    char *err_log = (char *)malloc(err_log_max_length);

    GLsizei err_log_length = 0;
    glGetShaderInfoLog(shader, err_log_max_length, &err_log_length, err_log);
    printf("%s\n", err_log);
}

void get_program_err(GLuint program) {
    GLint err_log_max_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &err_log_max_length);
    char *err_log = (char *)malloc(err_log_max_length);

    GLsizei err_log_length = 0;
    glGetProgramInfoLog(program, err_log_max_length, &err_log_length, err_log);
    printf("%s\n", err_log);
}

GLint build_shader(const char *file_string, GLenum shader_type) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &file_string, NULL);
    glCompileShader(shader);
    
    GLint compile_success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_success);
    
    if (!compile_success) {
        if (shader_type == GL_VERTEX_SHADER) {
            printf("Vertex Shader %d compile error!\n", shader);
            get_shader_err(shader);
            return 0;
        } else if (shader_type == GL_FRAGMENT_SHADER) {
            printf("Fragment Shader %d compile error!\n", shader);
            get_shader_err(shader);
            return 0;
        }
    }

    return shader;
}

GLuint load_and_build_program(char *vert_file, char *frag_file) {
    GLuint shader_program = glCreateProgram();
    
    GLint vert_shader = build_shader(vert_file, GL_VERTEX_SHADER);
    GLint frag_shader = build_shader(frag_file, GL_FRAGMENT_SHADER);

    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);

    glLinkProgram(shader_program);

    GLint is_linked = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &is_linked);
    if (!is_linked) {
        printf("Program %d link error!\n", shader_program);
        get_program_err(shader_program);
        return 0;
    }

    return shader_program;
}
#else
GLuint load_and_build_program(char *vert_file, char *frag_file) {
    GLuint shader_program = glCreateProgram();

    GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint f_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(v_shader, 1, &vert_file, NULL);
    glShaderSource(f_shader, 1, &frag_file, NULL);

    glCompileShader(v_shader);
    glCompileShader(f_shader);

    glAttachShader(shader_program, v_shader);
    glAttachShader(shader_program, f_shader);

    glLinkProgram(shader_program);

    return shader_program;
}
#endif //DEBUG

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_CLOSE 
       || uMsg == WM_DESTROY 
       || (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE) 
       || (uMsg == WM_CHAR && wParam == VK_ESCAPE)) {

        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void WinMainCRTStartup() {
    int res = WinMain(GetModuleHandle(0), 0, 0, 0);
    ExitProcess(res);
}

DWORD WINAPI AudioThread(LPVOID lpParam) {
    PlaySound(TEXT("SystemStart"), NULL, SND_ALIAS);
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc;

    ZeroMemory(&wc, sizeof(WNDCLASS));
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hPrevInstance;
    wc.lpszClassName = "test raw";

    static RECT rec = {0, 0, 800, 600};
    const static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 
        1, 
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 
        PFD_TYPE_RGBA,
        16, 
        0, 0, 0, 0, 0, 0,
        0, 
        0, 
        0, 
        0, 0, 0, 0,
        16,
        0,
        0,
        PFD_MAIN_PLANE,
        0, 0, 0, 0 
    };

    RegisterClass(&wc);
    const int dws = WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
    AdjustWindowRect(&rec, dws, 0);
    HWND hWnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, wc.lpszClassName, "test", dws,
                               (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
                               (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
                               rec.right-rec.left, rec.bottom-rec.top, 0, 0, hPrevInstance, 0);

    HDC hDC = GetDC(hWnd);
    int pix_fmt = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pix_fmt, &pfd);

    HGLRC hRC = wglCreateContext(hDC);

    int ret = wglMakeCurrent(hDC, hRC);

    gl_lite_init();

#ifdef DEBUG
    const GLubyte *gl_version = glGetString(GL_VERSION);
    printf("GL_VERSION: %s\n", gl_version);
#endif //DEBUG

    char *vert_file = 
#include "test.vert"
    ;
    char *frag_file = 
#include "test.frag"
    ;

    GLuint program = load_and_build_program(vert_file, frag_file);
    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    float vert_buff[] = {
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,

         1.0f, -1.0f,
        -1.0f, -1.0f,
        -1.0f,  1.0f,
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_buff), vert_buff, GL_STATIC_DRAW);

    GLint pos_attr = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(pos_attr);
    glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, 0, 0);

    GLint time_u = glGetUniformLocation(program, "time");

    LPDWORD thread_id = NULL;
    CreateThread(NULL, 0, AudioThread, NULL, 0, thread_id);

	bool running = true;
	while (running) {
        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        float time = 0.0025f * (float)(GetTickCount() & 65535);
        glUniform1f(time_u, time);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        SwapBuffers(hDC);
	}

	return 0;
}
