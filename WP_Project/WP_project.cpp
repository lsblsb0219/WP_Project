#include <windows.h>
#include "resource.h"

RECT rect;

int ballX, ballY, ballX2, ballY2, ellipseX, ellipseY, ellipseX2, ellipseY2;
int see_bw, see_bh, bWidth, bHeight, bWidth2, bHeight2;
int currentBitmapIndex = 0;
bool showMessageBox;
bool showMessage = false; // 메시지를 표시할지 여부
LPCWSTR message;
UINT_PTR messageTimerID = 1; // 타이머 ID
UINT_PTR respawnTimerID = 3; // 적 재등장을 위한 타이머 ID

// 주인공 체력 변수
int playerHealth = 3; // 초기 체력 3

// 적관련 변수
bool enemyActive = false;
int enemyX, enemyY, enemyWidth, enemyHeight;
UINT_PTR enemyTimerID = 2;

void DrawBall(HDC hdc);
void DrawEnemy(HDC hdc);
void DrawHealth(HDC hdc);
void BACK_DrawMessageBox(HDC hdc, LPCWSTR message);
bool CheckCollision();

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

    WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); //--- 메뉴 id 등록

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

        // 원과 (주인)공 크기 크기 및 위치 설정
        ellipseX = 0, ellipseY = 270;
        ballX = 120; ballY = 390;

        // 비트맵 복사 크기
        bWidth = rect.left, bHeight = rect.top;
        bWidth2 = rect.right - rect.left, bHeight2 = rect.bottom - rect.top;

        // VIP 비트맵의 너비 정보를 얻음
        GetObject(hBitmap, sizeof(BITMAP), &bit);

        // 적의 초기 위치와 크기 설정
        enemyWidth = 60;
        enemyHeight = 60;
        enemyX = rect.right - enemyWidth;
        enemyY = ballY; // 주인공의 Y 위치에 맞춰 초기화

        break;

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);

        hMemDC = CreateCompatibleDC(hDC);
        hMemDC2 = CreateCompatibleDC(hDC);

        GetClientRect(hWnd, &rect);

        // 비트맵 복사 크기
        see_bw = rect.right - rect.left, see_bh = rect.bottom - rect.top;    // 1300 800

        // 비트맵 불러오기
        SelectObject(hMemDC, hBitmap[currentBitmapIndex]);
        StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, bWidth, bHeight, bWidth2, bHeight2, SRCCOPY);

        // 검정색 비트맵 준비
        SelectObject(hMemDC2, hBitmap2);
        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        RECT blackRect = { 0, 0, see_bw, see_bh };
        FillRect(hMemDC2, &blackRect, blackBrush);
        DeleteObject(blackBrush);

        // 검정색 비트맵을 원 부분을 제외한 나머지에만 그리기
        BitBlt(hDC, 0, 0, see_bw, see_bh, hMemDC2, 0, 0, SRCCOPY); // 전체를 검정색으로 채움

        // 흰색 원 그리기
        ellipseX2 = ellipseX + 300, ellipseY2 = ellipseY + 300;
        HRGN hRgn = CreateEllipticRgn(ellipseX, ellipseY, ellipseX2, ellipseY2);
        SelectClipRgn(hDC, hRgn);
        StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, bWidth, bHeight, bWidth2, bHeight2, SRCCOPY); // VIP 비트맵을 원 부분에만 덮어 씀
        SelectClipRgn(hDC, NULL);
        DeleteObject(hRgn);

        // 테두리만 있는 원 그리기
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255)); // 흰색 테두리 펜
        HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH)); // 내부를 비우는 브러시

        Ellipse(hDC, ellipseX, ellipseY, ellipseX2, ellipseY2);

        SelectObject(hDC, hOldBrush);
        SelectObject(hDC, oldPen);
        DeleteObject(hPen);

        // Draw the message if needed
        if (showMessage) {
            message = L"더이상 나아갈수 없다.";
            SetTextColor(hDC, RGB(255, 0, 0));
            SetBkMode(hDC, TRANSPARENT);
            SIZE size;
            GetTextExtentPoint32(hDC, message, lstrlen(message), &size);
            TextOut(hDC, (rect.right - size.cx) / 2, rect.bottom - size.cy - 10, message, lstrlen(message));
        }

        DrawBall(hDC);

        if (showMessageBox) {
            BACK_DrawMessageBox(hDC, message);
        }

        if (enemyActive && currentBitmapIndex == 1) { // 적이 두 번째 맵에서만 나타나도록
            DrawEnemy(hDC); // 여기서 hDC를 사용해야 화면에 그려집니다.
        }

        // Draw player health
        DrawHealth(hDC);

        DeleteDC(hMemDC);
        DeleteDC(hMemDC2);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_KEYDOWN:
        switch (wParam) {
        case 'A': // 왼쪽
        case 'a':
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
        case 'D': // 오른쪽
        case 'd':
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
        case 'W': // 위쪽
        case 'w':
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
        case 'S': // 아래쪽
        case 's':
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
        if (CheckCollision() && enemyActive) {
            playerHealth--;
            enemyActive = false;
            KillTimer(hWnd, enemyTimerID);
            if (playerHealth <= 0) {
                // 게임 오버 처리
                MessageBox(hWnd, L"게임 오버", L"알림", MB_OK);
                PostQuitMessage(0);
            }
            else {
                // 5초 후에 적 재등장
                SetTimer(hWnd, respawnTimerID, 5000, NULL);
            }
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
            else if (ballX <= rect.left && currentBitmapIndex - 1 > -1) {
                if (showMessageBox == false) {
                    message = L"문이 잠겨있다. 다시 돌아갈 수 없는 것 같다.          ( 창닫기 F )";
                    showMessageBox = true; // 메시지 박스 활성화
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
        if (wParam == messageTimerID) {
            showMessage = false;
            KillTimer(hWnd, messageTimerID);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        // 적
        if (wParam == enemyTimerID) {
            int deltaX = ballX - enemyX;
            int deltaY = ballY - enemyY;

            if (abs(deltaX) > abs(deltaY)) {
                enemyX += (deltaX > 0) ? 5 : -5;
            }
            else {
                enemyY += (deltaY > 0) ? 5 : -5;
            }

            if (CheckCollision() && enemyActive) {
                playerHealth--;
                enemyActive = false;
                KillTimer(hWnd, enemyTimerID);
                if (playerHealth <= 0) {
                    // 게임 오버 처리
                    MessageBox(hWnd, L"게임 오버", L"알림", MB_OK);
                    PostQuitMessage(0);
                }
                else {
                    // 5초 후에 적 재등장
                    SetTimer(hWnd, respawnTimerID, 5000, NULL);
                }
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        if (wParam == respawnTimerID) {
            KillTimer(hWnd, respawnTimerID);
            if (currentBitmapIndex == 1) { // 2번째 맵에서만 재등장
                enemyActive = true;
                enemyX = rect.right - enemyWidth; // 오른쪽에서부터 나타나도록 초기 위치 설정
                enemyY = ballY; // 주인공의 Y 위치에 맞춰 초기화
                SetTimer(hWnd, enemyTimerID, 50, NULL); // 적의 움직임을 위한 타이머 시작
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    // 적 활성화 조건 체크 (2번째 맵에서만 활성화)
    if (currentBitmapIndex == 1 && bWidth2 > 800 && !enemyActive) { // bWidth2가 800을 초과할 때 적 활성화
        enemyActive = true;
        enemyX = rect.right - enemyWidth; // 오른쪽에서부터 나타나도록 초기 위치 설정
        enemyY = ballY; // 주인공의 Y 위치에 맞춰 초기화
        SetTimer(hWnd, enemyTimerID, 50, NULL); // 적의 움직임을 위한 타이머 시작
    }
    // 적 비활성화 조건 체크 (다른 맵으로 이동할 때)
    if (currentBitmapIndex != 1 && enemyActive) {
        enemyActive = false;
        KillTimer(hWnd, enemyTimerID);
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

void BACK_DrawMessageBox(HDC hdc, LPCWSTR message)
{
    RECT messageBoxRect;
    messageBoxRect.left = 10;
    messageBoxRect.top = rect.bottom - 60;
    messageBoxRect.right = rect.right - 10;
    messageBoxRect.bottom = rect.bottom - 10;

    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // 하얀색 브러시
    FillRect(hdc, &messageBoxRect, hBrush);
    DeleteObject(hBrush);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0)); // 검정색 텍스트
    DrawText(hdc, message, -1, &messageBoxRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DrawEnemy(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(139, 0, 0)); // 검붉은 색
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Rectangle(hdc, enemyX, enemyY, enemyX + enemyWidth, enemyY + enemyHeight);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}

void DrawHealth(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // 빨간색 브러시
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    for (int i = 0; i < playerHealth; i++) {
        POINT points[3] = {
            {20 + (i * 30), 20},
            {30 + (i * 30), 40},
            {10 + (i * 30), 40}
        };
        Polygon(hdc, points, 3);
    }

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}

bool CheckCollision() {
    return ballX2 > enemyX && ballX < enemyX + enemyWidth &&
        ballY2 > enemyY && ballY < enemyY + enemyHeight;
}
