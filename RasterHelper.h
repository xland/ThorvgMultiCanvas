#pragma once
#include <Windows.h>
#include <stdint.h>
#include <thorvg.h>
class RasterHelper
{
	public:
		RasterHelper(HWND hwnd);
		~RasterHelper();
		void resize();
		void blitToScreen(HDC hdc);
		tvg::Canvas* getCanvas();
	private:
		HWND hwnd;
		std::unique_ptr<tvg::SwCanvas> canvas;
		uint32_t* buffer;
		uint32_t w, h;
};

