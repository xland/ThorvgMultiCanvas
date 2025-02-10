#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>
#include <thorvg.h>
#include "MainWindow.h"

HDC g_hdc;
HGLRC g_hrc;

void createOpenGLContext(HWND hwnd) {
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
        PFD_TYPE_RGBA, // Color type
        32,           // Preferred color depth
        0, 0, 0, 0, 0, 0,
        0,             // No alpha buffer
        0,             // Shift bit ignored
        0,             // No accumulation buffer
        0, 0, 0, 0,    // Accumulation bits ignored
        24,            // 24-bit Z-buffer
        8,             // 8-bit stencil buffer
        0,             // No auxiliary buffer
        PFD_MAIN_PLANE,// Main layer
        0,             // Reserved
        0, 0, 0        // Layer masks ignored
    };

    g_hdc = GetDC(hwnd);
    int pixelFormat = ChoosePixelFormat(g_hdc, &pfd);
    SetPixelFormat(g_hdc, pixelFormat, &pfd);

    g_hrc = wglCreateContext(g_hdc);
    wglMakeCurrent(g_hdc, g_hrc);
}

void cleanupOpenGLContext() {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(g_hrc);
    ReleaseDC(GetForegroundWindow(), g_hdc);
}

class GLFrameBuffer {
public:
    GLuint fbo;
    GLuint texture;

    GLFrameBuffer(uint32_t width, uint32_t height) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~GLFrameBuffer() {
        if (fbo) glDeleteFramebuffers(1, &fbo);
        if (texture) glDeleteTextures(1, &texture);
    }

    void blitToScreen(uint32_t posX, uint32_t posY, uint32_t width, uint32_t height) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height, posX, posY, posX + width, posY + height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
};

MainWindow::MainWindow()
{
	initWinPosSize();
	initImgs();
    initWindow();
}

MainWindow::~MainWindow()
{
	
}
void MainWindow::initWindow()
{
    WNDCLASSEX wcx{};
    auto hinstance = GetModuleHandle(NULL);
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcx.lpfnWndProc = &MainWindow::routeWinMsg;
    wcx.cbWndExtra = sizeof(MainWindow*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_CROSS);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = L"ScreenCapture";
    RegisterClassEx(&wcx);
    auto style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
    hwnd = CreateWindowEx(NULL ,L"ScreenCapture", L"ScreenCapture", style, x, y, w, h, NULL, NULL, hinstance, static_cast<LPVOID>(this));
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));


}
void MainWindow::show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetCursor(LoadCursor(nullptr, IDC_ARROW));

}
void MainWindow::initWinPosSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

void MainWindow::initImgs()
{
}

void MainWindow::paint()
{

}
LRESULT MainWindow::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto obj = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (!obj) {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    switch (msg)
    {
        case WM_NCCALCSIZE:
        {
            if (wParam == TRUE) {
                NCCALCSIZE_PARAMS* pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                pncsp->rgrc[0] = pncsp->rgrc[1]; //窗口客户区覆盖整个窗口
                return 0; //确认改变窗口客户区
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        case WM_CLOSE:
        {
            DestroyWindow(hWnd);
            return 0;
        }
        case WM_DESTROY:
        {
            SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            UnregisterClass(L"ScreenCapture", nullptr);
            return 0;
        }
        default:
        {
            return obj->processWinMsg(msg, wParam, lParam);
        }
    }
}
LRESULT MainWindow::processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_MOVE: {
            x = LOWORD(lParam);
            y = HIWORD(lParam);
            return 0;
        }
        case WM_PAINT: {
            paint();
            PAINTSTRUCT ps;
            auto dc = BeginPaint(hwnd, &ps);
            //BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, h * 4 * w, 0, 0, 0, 0 };
            //SetDIBitsToDevice(dc, 0, 0, w, h, 0, 0, 0, h, imgBoard.bits(), &bmi, DIB_RGB_COLORS);
            ReleaseDC(hwnd, dc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            //auto e = Util::createMouseEvent(lParam, QEvent::MouseButtonPress);
            //mousePress(&e);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            //auto e = Util::createMouseEvent(lParam, QEvent::MouseMove);
            //if (wParam & MK_LBUTTON) {
            //    mouseDrag(&e);
            //}
            //else {
            //    mouseMove(&e);
            //}
            return 0;
        }
        case WM_LBUTTONDBLCLK:
        {
            //auto e = Util::createMouseEvent(lParam, QEvent::MouseButtonDblClick);
            //mouseDBClick(&e);
            return 0;
        }
        case WM_LBUTTONUP:
        {
            //auto e = Util::createMouseEvent(lParam, QEvent::MouseButtonRelease);
            //mouseRelease(&e);
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            //auto e = Util::createMouseEvent(lParam, QEvent::MouseButtonPress, Qt::MouseButton::RightButton);
            //mousePressRight(&e);
            return 0;
        }
        case WM_KEYDOWN:
        {
            if (wParam == VK_BACK || wParam == VK_DELETE) {
                //removeShape();
                return true;
            }
            else if (wParam == VK_ESCAPE) {
                //escPress();
                return true;
            }
            else if (wParam == VK_LEFT) {
                //moveByKey(0);
                return true;
            }
            else if (wParam == VK_UP) {
                //moveByKey(1);
                return true;
            }
            else if (wParam == VK_RIGHT) {
                //moveByKey(2);
                return true;
            }
            else if (wParam == VK_DOWN) {
                //moveByKey(3);
                return true;
            }
            else if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) {
                if (wParam == 'Z') {
                    //undo(); 撤销
                    return true;
                }
                else if (wParam == 'Y') {
                    //redo(); 重做
                    return true;
                }
                else if (wParam == 'H') {
                    //copyColor(0); Hex
                    return true;
                }
                else if (wParam == 'R') {
                    //copyColor(1); RGB
                    return true;
                }
            }
            break;
        }
        default: {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }
    return 0;
}
void MainWindow::mouseMove(POINT* event)
{
}
void MainWindow::mouseDrag(POINT* event) {

}
void MainWindow::mousePress(POINT* event)
{
}