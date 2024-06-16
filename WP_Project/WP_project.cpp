#include <windows.h>
#include "resource.h"

RECT rect;

int ballX, ballY, ballX2, ballY2, ellipseX, ellipseY, ellipseX2, ellipseY2, scan_ellipseX2, scan_ellipseX, scan_ellipseY2, scan_ellipseY;
int see_bw, see_bh, bWidth, bHeight, bWidth2, bHeight2;
int currentBitmapIndex = 0;
bool showMessageBox ;
bool showMessage = false; // �޽����� ǥ������ ����
LPCWSTR message;
UINT_PTR messageTimerID = 1; // Ÿ�̸� ID
bool showScanEllipse = false; // ��ĵ ���� ǥ������ ����
UINT_PTR Scan_ellipse = 2;
int size_cheack; // ��ĵ �� ũ�� ���� Ȯ��

void DrawBall(HDC hdc);
void DrawMessageBox(HDC hdc, LPCWSTR message);
void DrawScanEllipse(HDC hdc);

HINSTANCE g_hlnst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Windows program Project";
HBITMAP hBitmap[4], hBitmap2, hGunBitmap;
BITMAP bit, bit2, bitGun;

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

	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); //--- �޴� id ���

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

    switch (iMessage) {
    case WM_CREATE:
        GetClientRect(hWnd, &rect);

        hBitmap[0] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_VIPfloor));
        hBitmap[1] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_Restaurant));
        hBitmap[2] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_Roomfloor));
        hBitmap[3] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_Normalfloor));
        hBitmap2 = CreateCompatibleBitmap(GetDC(hWnd), 1300, 840);

        // �ʱ� ��ġ ����
        ellipseX = 0, ellipseY = 270;
        ballX = 120; ballY = 390;

        // ��Ʈ�� ���� ũ�� ����
        bWidth = rect.left, bHeight = rect.top;
        bWidth2 = rect.right - rect.left, bHeight2 = rect.bottom - rect.top;

        // VIP ��Ʈ���� �ʺ� ���� ���
        GetObject(hBitmap[0], sizeof(BITMAP), &bit);

        // �� �̹��� �ε�
        hGunBitmap = (HBITMAP)LoadImage(g_hlnst, MAKEINTRESOURCE(IDB_Gun), IMAGE_BITMAP, 0, 0, 0);
        GetObject(hGunBitmap, sizeof(BITMAP), &bitGun);

        size_cheack = 0;

        break;

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);

        hMemDC = CreateCompatibleDC(hDC);
        hMemDC2 = CreateCompatibleDC(hDC);

        GetClientRect(hWnd, &rect);

        // ��Ʈ�� ���� ũ��
        see_bw = rect.right - rect.left, see_bh = rect.bottom - rect.top;

        // ���� ��Ʈ�� �׸���
        SelectObject(hMemDC, hBitmap[currentBitmapIndex]);
        StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, bWidth, bHeight, bWidth2, bHeight2, SRCCOPY);

        // ������ ��� �׸���
        SelectObject(hMemDC2, hBitmap2);
        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        RECT blackRect = { 0, 0, see_bw, see_bh };
        FillRect(hMemDC2, &blackRect, blackBrush);
        DeleteObject(blackBrush);

        // ������ ��濡�� �� �κ��� ������ �κ� �׸���
        BitBlt(hDC, 0, 0, see_bw, see_bh, hMemDC2, 0, 0, SRCCOPY);

        // �� �׸���
        ellipseX2 = ellipseX + 300, ellipseY2 = ellipseY + 300;
        HRGN hRgn = CreateEllipticRgn(ellipseX, ellipseY, ellipseX2, ellipseY2);
        SelectClipRgn(hDC, hRgn);
        StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, bWidth, bHeight, bWidth2, bHeight2, SRCCOPY);
        SelectClipRgn(hDC, NULL);
        DeleteObject(hRgn);

        // ��ĵ �� �׸��� (���)
        if (showScanEllipse) {
            DrawScanEllipse(hDC);
        }

        // �� �̹��� �׸���
        if (currentBitmapIndex == 0) {
            int gunX = (rect.right - bitGun.bmWidth) / 2;
            int gunY = (rect.bottom - bitGun.bmHeight) / 2;

            BitBlt(hDC, gunX, gunY, bitGun.bmWidth, bitGun.bmHeight, hMemDC, 0, 0, SRCCOPY);

            // ���� �� �̹����� ��ġ�� �Ķ��� �׵θ� �׸���
            if (ellipseX2 > gunX && ellipseX < gunX + bitGun.bmWidth && ellipseY2 > gunY && ellipseY < gunY + bitGun.bmHeight) {
                HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
                HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

                Rectangle(hDC, gunX, gunY, gunX + bitGun.bmWidth, gunY + bitGun.bmHeight);

                SelectObject(hDC, hOldBrush);
                SelectObject(hDC, oldPen);
                DeleteObject(hPen);
            }
        }

        DeleteDC(hMemDC);
        DeleteDC(hMemDC2);

        // �߰� �׸��� �Լ� ȣ��
        DrawBall(hDC);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_KEYDOWN:
        switch (wParam) {
        case 'a':
        case 'A':
            // �������� �̵�
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

        case 'd':
        case 'D':
            // ���������� �̵�
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

        case 'w':
        case 'W':
            // ���� �̵�
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

        case 's':
        case 'S':
            // �Ʒ��� �̵�
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
        case 'e':
		case 'E':
			showScanEllipse = true; // ��ĵ �� Ȱ��ȭ
            scan_ellipseX = ellipseX, scan_ellipseY = ellipseY;
            scan_ellipseX2 = scan_ellipseX + 300, scan_ellipseY2 = scan_ellipseY + 300;
			SetTimer(hWnd, Scan_ellipse, 50, NULL); // Ÿ�̸� ����
			break;

        case 'f':
        case 'F':
            // ��Ʈ�� ���� �� �޽��� ó��
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
            else if (ballX <= rect.left && currentBitmapIndex - 1 > -1) {
                if (showMessageBox == false) {
                    message = L"���� ����ִ�. �ٽ� ���ư� �� ���� �� ����.          ( â�ݱ� F )";
                    showMessageBox = true; // �޽��� �ڽ� Ȱ��ȭ
                }
                else {
                    showMessageBox = false;
                }
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;

        case 'q':
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_TIMER:
    {
            if (showScanEllipse) {
            scan_ellipseX -= 10;
            scan_ellipseY -= 10;
            scan_ellipseX2 += 10;
            scan_ellipseY2 += 10;

            size_cheack += 10;

            if (size_cheack >= 500) {
                KillTimer(hWnd, Scan_ellipse);
                showScanEllipse = false;
                size_cheack = 0;
            }

            InvalidateRect(hWnd, NULL, TRUE);
        }

        if (wParam == messageTimerID) {
            showMessage = false;
            KillTimer(hWnd, messageTimerID);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }

    case WM_DESTROY:
        for (int i = 0; i < 4; ++i) {
            DeleteObject(hBitmap[i]);
        }
        DeleteObject(hBitmap2);
        DeleteObject(hGunBitmap);
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

void DrawMessageBox(HDC hdc, LPCWSTR message)
{
    RECT messageBoxRect;
    messageBoxRect.left = 10;
    messageBoxRect.top = rect.bottom - 60;
    messageBoxRect.right = rect.right - 10;
    messageBoxRect.bottom = rect.bottom - 10;

    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // �Ͼ�� �귯��
    FillRect(hdc, &messageBoxRect, hBrush);
    DeleteObject(hBrush);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0)); // ������ �ؽ�Ʈ
    DrawText(hdc, message, -1, &messageBoxRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DrawScanEllipse(HDC hdc)
{
    HRGN scanHRgn = CreateEllipticRgn(scan_ellipseX, scan_ellipseY, scan_ellipseX2, scan_ellipseY2);
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255)); // ��� �귯��
    FrameRgn(hdc, scanHRgn, whiteBrush, 3, 3); // �ܰ����� �׸��� ���� FrameRgn ���
    DeleteObject(scanHRgn);
    DeleteObject(whiteBrush);
}
