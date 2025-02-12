#include "GLHelper.h"

namespace {
    #define GL_FRAMEBUFFER 0x8D40
    #define GL_BGRA 0x80E1
    #define GL_COLOR_ATTACHMENT0 0x8CE0
    #define GL_READ_FRAMEBUFFER 0x8CA8
    #define GL_DRAW_FRAMEBUFFER 0x8CA9
    #define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
    #define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
    #define WGL_CONTEXT_PROFILE_MASK_ARB  0x9126
    #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

    typedef void (*PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
    typedef void (*PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
    typedef void (WINAPI* PFNGLBINDTEXTUREEXTPROC) (GLenum target, GLuint texture);
    typedef void (WINAPI* PFNGLDELETETEXTURESEXTPROC) (GLsizei n, const GLuint* textures);
    typedef void (WINAPI* PFNGLGENTEXTURESEXTPROC) (GLsizei n, GLuint* textures);
    typedef void (WINAPI* PFNGLGENFRAMEBUFFERSPROC) (GLsizei n, GLuint* framebuffers);
    typedef void (WINAPI* PFNGLBINDFRAMEBUFFERPROC) (GLenum target, GLuint framebuffer);
    typedef void (WINAPI* PFNGLDELETEFRAMEBUFFERSPROC) (GLsizei n, const GLuint* framebuffers);
    typedef void (WINAPI* PFNGLFRAMEBUFFERTEXTURE2DPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    typedef void (WINAPI* PFNGLBLITFRAMEBUFFERPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
    typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int* attribList);

    PFNGLBINDTEXTUREEXTPROC fglBindTexture = nullptr;
    PFNGLDELETETEXTURESEXTPROC fglDeleteTextures = nullptr;
    PFNGLGENTEXTURESEXTPROC fglGenTextures = nullptr;
    PFNGLGENFRAMEBUFFERSPROC fglGenFramebuffers = nullptr;
    PFNGLBINDFRAMEBUFFERPROC fglBindFramebuffer = nullptr;
    PFNGLDELETEFRAMEBUFFERSPROC fglDeleteFramebuffers = nullptr;
    PFNGLFRAMEBUFFERTEXTURE2DPROC fglFramebufferTexture2D = nullptr;
    PFNGLBLITFRAMEBUFFERPROC fglBlitFramebuffer = nullptr;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
}

GLHelper::GLHelper(HWND hwnd):hwnd{hwnd}
{
    initContext();
}

GLHelper::~GLHelper()
{
    if (fbo) fglDeleteFramebuffers(1, &fbo);
    if (texture) fglDeleteTextures(1, &texture);
    wglMakeCurrent(nullptr, nullptr); 
    wglDeleteContext(context);
}

void GLHelper::resize()
{
    fglBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (fbo) {
        fglDeleteFramebuffers(1, &fbo);
    }
    if (texture) {
        fglDeleteTextures(1, &texture);
    }
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        w = rect.right - rect.left;
        h = rect.bottom - rect.top;
    }

    fglGenFramebuffers(1, &fbo);
    fglBindFramebuffer(GL_FRAMEBUFFER, fbo);
    fglGenTextures(1, &texture);
    fglBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fglFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    fglBindFramebuffer(GL_FRAMEBUFFER, 0);
    fglBindTexture(GL_TEXTURE_2D, 0);
    if (canvas) {
        canvas->clear(true);
        canvas.reset();
    }
    canvas = tvg::GlCanvas::gen();
    canvas->target(fbo, w, h);
}

void GLHelper::initContext()
{
    HDC hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);
    HGLRC oldContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, oldContext);
    fglBindTexture = (PFNGLBINDTEXTUREEXTPROC)wglGetProcAddress("glBindTexture");
    fglDeleteTextures = (PFNGLDELETETEXTURESEXTPROC)wglGetProcAddress("glDeleteTextures");
    fglGenTextures = (PFNGLGENTEXTURESEXTPROC)wglGetProcAddress("glGenTextures");
    fglBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
    fglGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
    fglDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
    fglFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
    fglBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer");
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    HGLRC newContext = wglCreateContextAttribsARB(hdc, 0, attribs);
    if (newContext) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(oldContext);
        context = newContext;
    }
    else {
        context = oldContext;
    }
    wglMakeCurrent(hdc, context);
}

void GLHelper::blitToScreen(HDC hdc)
{
    fglBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    fglBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    fglBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    SwapBuffers(hdc);
}

tvg::Canvas* GLHelper::getCanvas()
{
    return canvas.get();
}
