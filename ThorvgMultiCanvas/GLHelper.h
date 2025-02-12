#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdint.h>
#include <thorvg.h>

class GLHelper
{
public:
	GLHelper(HWND hwnd);
	~GLHelper();
	void resize();
	void initContext();
	void blitToScreen(HDC hdc);
	tvg::Canvas* getCanvas();
private:
	GLuint fbo;
	GLuint texture;
	HWND hwnd;
	HGLRC context;
	std::unique_ptr<tvg::GlCanvas> canvas;
	uint32_t w, h;
};

