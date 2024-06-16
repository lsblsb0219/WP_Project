#include <windows.h>
#include "resource.h"

#pragma comment(lib, "Msimg32.lib")

RECT rect;

int ballX, ballY, ballX2, ballY2, ellipseX, ellipseY, ellipseX2, ellipseY2;
int see_bw, see_bh, bWidth, bHeight, bWidth2, bHeight2;
int currentBitmapIndex = 0;
bool isKeyPressed;
bool showMessage = false; // �޽����� ǥ������ ����
UINT_PTR messageTimerID = 1; // Ÿ�̸� ID

void DrawBall(HDC hdc);

HINSTANCE g_hlnst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Windows program Project";
HBITMAP hBitmap[4], hBitmap2;
BITMAP bit, bit2;

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
	HDC hDC, hMemDC, hMemDC2;
	PAINTSTRUCT ps;
	HBITMAP oldBitmap, oldBitmap2;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &rect);

		hBitmap[0] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_VIPfloor));
		hBitmap[1] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_Restaurant));
		hBitmap[2] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_Roomfloor));
		hBitmap[3] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_Normalfloor));
		hBitmap2 = CreateCompatibleBitmap(GetDC(hWnd), 1300, 840);

		// ���� (����)�� ũ�� ũ�� �� ��ġ ����
		ellipseX = 0, ellipseY = 270;
		ballX = 120; ballY = 390;

		// ��Ʈ�� ���� ũ��
		bWidth = rect.left, bHeight = rect.top;
		bWidth2 = rect.right - rect.left, bHeight2 = rect.bottom - rect.top;

		// VIP ��Ʈ���� �ʺ� ������ ����
		GetObject(hBitmap, sizeof(BITMAP), &bit);

		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);

		hMemDC = CreateCompatibleDC(hDC);
		hMemDC2 = CreateCompatibleDC(hDC);

		GetClientRect(hWnd, &rect);

		// ��Ʈ�� ���� ũ��
		see_bw = rect.right - rect.left, see_bh = rect.bottom - rect.top;	// 1300 800

		// ��Ʈ�� �ҷ�����
		SelectObject(hMemDC, hBitmap[currentBitmapIndex]);
		StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, bWidth, bHeight, bWidth2, bHeight2, SRCCOPY);


		// ������ ��Ʈ�� �غ�
		SelectObject(hMemDC2, hBitmap2);
		HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
		RECT blackRect = { 0, 0, see_bw, see_bh };
		FillRect(hMemDC2, &blackRect, blackBrush);
		DeleteObject(blackBrush);

		// ������ ��Ʈ���� �� �κ��� ������ ���������� �׸���
		BitBlt(hDC, 0, 0, see_bw, see_bh, hMemDC2, 0, 0, SRCCOPY); // ��ü�� ���������� ä��

		// ��� �� �׸���
		ellipseX2 = ellipseX + 300, ellipseY2 = ellipseY + 300;
		HRGN hRgn = CreateEllipticRgn(ellipseX, ellipseY, ellipseX2, ellipseY2);
		SelectClipRgn(hDC, hRgn);
		StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, bWidth, bHeight, bWidth2, bHeight2, SRCCOPY); // VIP ��Ʈ���� �� �κп��� ���� ��
		SelectClipRgn(hDC, NULL);
		DeleteObject(hRgn);

		// �׵θ��� �ִ� �� �׸���
		HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255)); // ��� �׵θ� ��
		HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH)); // ���θ� ���� �귯��

		Ellipse(hDC, ellipseX, ellipseY, ellipseX2, ellipseY2);

		SelectObject(hDC, hOldBrush);
		SelectObject(hDC, oldPen);
		DeleteObject(hPen);

		// Draw the message if needed
		if (showMessage) {
			LPCTSTR message = L"���̻� ���ư��� ����.";
			SetTextColor(hDC, RGB(255, 0, 0));
			SetBkMode(hDC, TRANSPARENT);
			SIZE size;
			GetTextExtentPoint32(hDC, message, lstrlen(message), &size);
			TextOut(hDC, (rect.right - size.cx) / 2, rect.bottom - size.cy - 10, message, lstrlen(message));
		}

		DeleteDC(hMemDC);
		DeleteDC(hMemDC2);

		DrawBall(hDC);

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			if (ballX >= rect.left && ballX2 <= rect.left / 2) {
				ballX -= 10;
				ellipseX -= 10;
			}
			else if (ballX >= rect.left && bWidth <= 0) {
				ballX -= 10;
				ellipseX -= 10;
			}
			else if (ballX2 >= rect.left / 2 && bWidth > 0) {
				bWidth -= 10;
				bWidth2 -= 10;
			}
			break;
		case VK_RIGHT:
			if (currentBitmapIndex == 0 || currentBitmapIndex == 2) {
				if (ballX2 <= rect.right && ballX2 <= rect.right / 2) {
					ballX += 10;
					ellipseX += 10;
				}
				else if (ballX2 <= rect.right && bWidth2 >= 2130) {
					ballX += 10;
					ellipseX += 10;
				}
				else if (ballX2 >= rect.right / 2 && bWidth2 < 2130) {
					bWidth += 10;
					bWidth2 += 10;
				}
			}
			else {
				if (ballX2 <= rect.right && ballX2 <= rect.right / 2) {
					ballX += 10;
					ellipseX += 10;
				}
				else if (ballX2 <= rect.right && bWidth2 >= 2130) {
					ballX += 10;
					ellipseX += 10;
				}
				else if (ballX2 >= rect.right / 2 && bWidth2 < 2130) {
					bWidth += 10;
					bWidth2 += 10;
				}
			}
			break;
		case VK_UP:
			if (currentBitmapIndex == 0 || currentBitmapIndex == 2) {
				if (ballY >= rect.top) {
					ballY -= 10;
					ellipseY -= 10;
				}
			}
			else {
				if (ballY > rect.top && bHeight >= 0) {
					ballY -= 10;
					ellipseY -= 10;
				}
				else if (ballY == rect.top && bHeight > 0) {
					bHeight -= 10;
					bHeight2 -= 10;
				}
			}
			break;
		case VK_DOWN:
			if (currentBitmapIndex == 0 || currentBitmapIndex == 2) {
				if (ballY2 <= rect.bottom) {
					ballY += 10;
					ellipseY += 10;
				}
			}
			else {
				if (ballY2 < rect.bottom && bHeight2 <= 1200) {
					ballY += 10;
					ellipseY += 10;
				}
				else if (ballY2 >= rect.bottom) {
					if (bHeight2 < 1200) {
						bHeight += 10;
						bHeight2 += 10;
					}
				}
			}
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_CHAR:
		switch (wParam) {
		case 'f':
			if (ballX2 >= rect.right && currentBitmapIndex <= 2) {
				currentBitmapIndex += 1;
				ellipseX = 0; ellipseY = 270;
				ballX = 120; ballY = 390;
				bWidth = 0; bWidth2 = 1300;

				if (currentBitmapIndex == 1 || currentBitmapIndex == 3) {
					bHeight = 250; bHeight2 = 1050;
				}
				else {
					bHeight = 0; bHeight2 = 800;
				}

				InvalidateRect(hWnd, NULL, TRUE);
			}
			else if (ballX2 >= rect.right && currentBitmapIndex == 3) {
				showMessage = true;
				SetTimer(hWnd, messageTimerID, 3000, NULL);
			}
			break;
		case 'q':
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_TIMER:
		if (wParam == messageTimerID) {
			showMessage = false;
			KillTimer(hWnd, messageTimerID);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

void DrawBall(HDC hdc)
{
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	ballX2 = ballX + 50, ballY2 = ballY + 50;
	Ellipse(hdc, ballX, ballY, ballX2, ballY2);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
}
