#include <windows.h>
#include "resource.h"

#pragma comment(lib, "Msimg32.lib")

int ballX, ballY;

void DrawBall(HDC hdc);

HINSTANCE g_hlnst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Windows program Project";
HBITMAP hBitmap, hBitmap2;

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
    BITMAP bit, bit2;
    RECT rect;

    ballX = 120; ballY = 390;

    switch (iMessage) {
    case WM_CREATE:
        hBitmap = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_VIPfloor));
        hBitmap2 = CreateCompatibleBitmap(GetDC(hWnd), 1300, 840);
        return 0;

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);

        hMemDC = CreateCompatibleDC(hDC);
        hMemDC2 = CreateCompatibleDC(hDC);

        GetClientRect(hWnd, &rect);

        int see_bw = rect.right - rect.left;
        int see_bh = rect.bottom - rect.top;
        int bWidth = rect.right - rect.left, bHeight = rect.bottom - rect.top;

        // VIPĭ ��Ʈ��
        SelectObject(hMemDC, hBitmap);
        GetObject(hBitmap, sizeof(BITMAP), &bit);
        StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, 0, 0, bWidth, bHeight, SRCCOPY);

        // ������ ��Ʈ�� �غ�
        SelectObject(hMemDC2, hBitmap2);
        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        RECT blackRect = { 0, 0, see_bw, see_bh };
        FillRect(hMemDC2, &blackRect, blackBrush);
        DeleteObject(blackBrush);

        // �� ũ�� ũ�� �� ��ġ ����
        int ellipse1 = 0, ellipse2 = 270, ellipse3 = ellipse1 + 300, ellipse4 = ellipse2 + 300;

        // ������ ��Ʈ���� �� �κ��� ������ ���������� �׸���
        BitBlt(hDC, 0, 0, see_bw, see_bh, hMemDC2, 0, 0, SRCCOPY); // ��ü�� ���������� ä��

        // �� �κи� �����ϱ�
        HRGN hRgn = CreateEllipticRgn(ellipse1, ellipse2, ellipse3, ellipse4);
        SelectClipRgn(hDC, hRgn);
        BitBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, 0, 0, SRCCOPY); // VIP ��Ʈ���� �� �κп��� ���� ��
        SelectClipRgn(hDC, NULL);
        DeleteObject(hRgn);

        // �׵θ��� �ִ� �� �׸���
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255)); // ��� �׵θ� ��
        HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH)); // ���θ� ���� �귯��

        Ellipse(hDC, ellipse1, ellipse2, ellipse3, ellipse4);

        SelectObject(hDC, hOldBrush);
        SelectObject(hDC, oldPen);
        DeleteObject(hPen);

        DeleteDC(hMemDC);
        DeleteDC(hMemDC2);

        DrawBall(hDC);

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

void DrawBall(HDC hdc)
{
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Ellipse(hdc, ballX, ballY, ballX + 50, ballY + 50);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}
