#include <Windows.h>
#include <iostream>
#include <thorvg.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "MainWindow.h"


MainWindow::MainWindow()
{
    //tvg::Initializer::init(tvg::CanvasEngine::Gl, 4);
    tvg::Initializer::init(tvg::CanvasEngine::Sw, 4);
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
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC | CS_BYTEALIGNCLIENT;
    wcx.lpfnWndProc = &MainWindow::routeWinMsg;
    wcx.cbWndExtra = sizeof(MainWindow*);
    wcx.hInstance = hinstance;
    wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_CROSS);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = L"ScreenCapture";
    RegisterClassEx(&wcx);
    auto style = WS_OVERLAPPEDWINDOW;
    hwnd = CreateWindowEx(NULL ,L"ScreenCapture", L"ScreenCapture", style, x, y, w, h, NULL, NULL, hinstance, NULL);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    //runGl(hwnd);
    //glHelper = std::make_unique<GLHelper>(hwnd);
    rasterHelper = std::make_unique<RasterHelper>(hwnd);

}
void MainWindow::show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetCursor(LoadCursor(nullptr, IDC_ARROW));

    HDC hdc = GetDC(hwnd);
    

    //glFbo.blitToScreen(0, 0, w, h);
    //SwapBuffers(hdc);

}
void MainWindow::initWinPosSize()
{
    //x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    //y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    //w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    //h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    x = 300;
    y = 300;
    w = 1000;
    h = 800;
}

void MainWindow::initImgs()
{
}

void MainWindow::paint()
{
    //auto canvas = glHelper->getCanvas();
    auto canvas = rasterHelper->getCanvas();
    auto bg = tvg::Shape::gen();
    bg->appendRect(w-200, h-200, 160, 160);
    bg->fill(116, 125, 255);
    canvas->push(std::move(bg));
    canvas->draw();
    canvas->sync();
}
LRESULT MainWindow::routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto obj = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (!obj) {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    switch (msg)
    {
        //case WM_NCCALCSIZE:
        //{
        //    if (wParam == TRUE) {
        //        NCCALCSIZE_PARAMS* pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
        //        pncsp->rgrc[0] = pncsp->rgrc[1]; //窗口客户区覆盖整个窗口
        //        return 0; //确认改变窗口客户区
        //    }
        //    return DefWindowProc(hWnd, msg, wParam, lParam);
        //}
        case WM_ERASEBKGND:
        {
            return TRUE;
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
        case WM_SIZE: {
            //todo minimize
            w = LOWORD(lParam);
            h = HIWORD(lParam);
            //glHelper->resize();
            rasterHelper->resize();
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            auto hdc = BeginPaint(hwnd, &ps);
            paint();
            //glHelper->blitToScreen(hdc);
            rasterHelper->blitToScreen(hdc);
            ReleaseDC(hwnd, hdc);
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