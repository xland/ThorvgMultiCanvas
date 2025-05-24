#include "RasterHelper.h"

RasterHelper::RasterHelper(HWND hwnd):hwnd{hwnd}
{

}

RasterHelper::~RasterHelper()
{
}

void RasterHelper::resize()
{
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        w = rect.right - rect.left;
        h = rect.bottom - rect.top;
    }
    if (buffer) {
        delete[] buffer;
    }
    buffer = new uint32_t[w * h];
    if (canvas) {
        canvas.reset();
    }
    canvas = std::unique_ptr<tvg::SwCanvas>(tvg::SwCanvas::gen());
    canvas->target(buffer,w, w, h, tvg::ColorSpace::ABGR8888S);
}

void RasterHelper::blitToScreen(HDC hdc)
{
    BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, h * 4 * w, 0, 0, 0, 0 };
    SetDIBitsToDevice(hdc, 0, 0, w, h, 0, 0, 0, h, buffer, &bmi, DIB_RGB_COLORS);
}

tvg::Canvas* RasterHelper::getCanvas()
{
    return canvas.get();
}
