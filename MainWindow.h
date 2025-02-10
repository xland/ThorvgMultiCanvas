#pragma once
#include <Windows.h>

class MainWindow
{
public:
	MainWindow();
    ~MainWindow();
	void show();
public:
    int x, y, w, h;
    HWND hwnd;
protected:
	void initWindow();
	void mousePress(POINT* event);
	void mousePressRight(POINT* event) {};
	void mouseDBClick(POINT* event) {};
	void mouseMove(POINT* event);
	void mouseDrag(POINT* event);
	void mouseRelease(POINT* event) {};
private:
	void initWinPosSize();
	void initImgs();
	void paint();
	static LRESULT CALLBACK routeWinMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK processWinMsg(UINT msg, WPARAM wParam, LPARAM lParam);
private:
};
