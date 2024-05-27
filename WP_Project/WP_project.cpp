#include <windows.h>
#include "resource.h"

HINSTANCE g_hlnst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Windows program Project";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE gPreblnstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hlnst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_HAND);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_QUESTION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1300, 840, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hDC, hMemDC, hDC2, hMemDC2;
	PAINTSTRUCT ps;
	HBITMAP hBitmap, oldBitmap, hBitmap2, oldBitmap2;
	BITMAP bit;
	RECT rect;

	switch (iMessage) {
	case WM_CREATE:
		return 0;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);

		hMemDC = CreateCompatibleDC(hDC);

		GetClientRect(hWnd, &rect);

		int see_bw = rect.right - rect.left;
		int see_bh = rect.bottom - rect.top;

		hBitmap = (HBITMAP)LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_VIPfloor));
		oldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		int bWidth = rect.right - rect.left, bHeight = rect.bottom - rect.top;

		GetObject(hBitmap, sizeof(BITMAP), &bit);

		StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, 0, 0, bWidth, bHeight, SRCCOPY);

		SelectObject(hMemDC, oldBitmap);
		DeleteDC(hMemDC);

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_CHAR:
		switch (wParam) {
		case 'q':
			PostQuitMessage(0);
			break;
		}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}
