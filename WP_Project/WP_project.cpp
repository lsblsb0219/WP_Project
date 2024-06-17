#include <windows.h>
#include "resource.h"

RECT rect;

int ballX, ballY, ballX2, ballY2, ellipseX, ellipseY, ellipseX2, ellipseY2, scan_ellipseX2, scan_ellipseX, scan_ellipseY2, scan_ellipseY;
int see_bw, see_bh, bWidth, bHeight, bWidth2, bHeight2;
int currentBitmapIndex = 0;
bool showMessageBox;
bool showMessage = false; // 메시지를 표시할지 여부
LPCWSTR message;
UINT_PTR messageTimerID = 1; // 타이머 ID

// 스캔
bool showScanEllipse = false; // 스캔 원을 표시할지 여부
UINT_PTR Scan_ellipse = 2;
int size_cheack; // 스캔 원 크기 제한 확인
UINT_PTR respawnTimerID = 3; // 적 재등장을 위한 타이머 ID
UINT_PTR gameOverTimerID = 4; // 게임 오버 후 재시작을 위한 타이머 ID

// 주인공 체력 변수
int playerHealth = 3; // 초기 체력 3

// 적 관련 변수
bool enemyActive = false;
int enemyX, enemyY, enemyWidth, enemyHeight;
UINT_PTR enemyTimerID = 2;

// 두 번째 적 관련 변수
bool enemy2Active = false;
int enemy2X, enemy2Y, enemy2Width, enemy2Height;
UINT_PTR enemy2TimerID = 5;

void DrawBall(HDC hdc);
void DrawMessageBox(HDC hdc, LPCWSTR message);
void DrawScanEllipse(HDC hdc);
void DrawEnemy(HDC hdc, int x, int y, int width, int height);
void DrawHealth(HDC hdc);
void DrawGameOver(HDC hdc);
bool CheckCollision(int enemyX, int enemyY, int enemyWidth, int enemyHeight);
void ResetGame();

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

    static bool gameOver = false; // 게임 오버 상태 변수 추가

    switch (iMessage) {
    case WM_CREATE:
        GetClientRect(hWnd, &rect);

        hBitmap[0] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_VIPfloor));
        hBitmap[1] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_Restaurant));
        hBitmap[2] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_Roomfloor));
        hBitmap[3] = LoadBitmap(g_hlnst, MAKEINTRESOURCE(ID_Normalfloor));
        hBitmap2 = CreateCompatibleBitmap(GetDC(hWnd), 1300, 840);

        // 초기 위치 설정
        ellipseX = 0, ellipseY = 270;
        ballX = 120; ballY = 390;

        // 비트맵 복사 크기 설정
        bWidth = rect.left, bHeight = rect.top;
        bWidth2 = rect.right - rect.left, bHeight2 = rect.bottom - rect.top;

        // VIP 비트맵의 너비 정보 얻기
        GetObject(hBitmap[0], sizeof(BITMAP), &bit);

        // 적의 초기 위치와 크기 설정
        enemyWidth = 60;
        enemyHeight = 60;
        enemyX = rect.right - enemyWidth;
        enemyY = ballY; // 주인공의 Y 위치에 맞춰 초기화

        // 두 번째 적 초기 위치와 크기 설정
        enemy2Width = 60;
        enemy2Height = 60;
        enemy2X = rect.right - enemy2Width;
        enemy2Y = ballY + 100; // 주인공의 Y 위치에 맞춰 초기화

        // 총 이미지 로드
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

        // 비트맵 복사 크기
        see_bw = rect.right - rect.left, see_bh = rect.bottom - rect.top;

        // 메인 비트맵 그리기
        SelectObject(hMemDC, hBitmap[currentBitmapIndex]);
        StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, bWidth, bHeight, bWidth2, bHeight2, SRCCOPY);

        // 검정색 배경 그리기
        SelectObject(hMemDC2, hBitmap2);
        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        RECT blackRect = { 0, 0, see_bw, see_bh };
        FillRect(hMemDC2, &blackRect, blackBrush);
        DeleteObject(blackBrush);

        // 검정색 배경에서 원 부분을 제외한 부분 그리기
        BitBlt(hDC, 0, 0, see_bw, see_bh, hMemDC2, 0, 0, SRCCOPY);

        // 원 그리기
        ellipseX2 = ellipseX + 300, ellipseY2 = ellipseY + 300;
        HRGN hRgn = CreateEllipticRgn(ellipseX, ellipseY, ellipseX2, ellipseY2);
        SelectClipRgn(hDC, hRgn);
        StretchBlt(hDC, 0, 0, see_bw, see_bh, hMemDC, bWidth, bHeight, bWidth2, bHeight2, SRCCOPY);
        SelectClipRgn(hDC, NULL);
        DeleteObject(hRgn);

        // 스캔 원 그리기 (흰색)
        if (showScanEllipse) {
            DrawScanEllipse(hDC);
        }

        // 총 이미지 그리기
        if (currentBitmapIndex == 0) {
            int gunX = (rect.right - bitGun.bmWidth) / 2;
            int gunY = (rect.bottom - bitGun.bmHeight) / 2;

            BitBlt(hDC, gunX, gunY, bitGun.bmWidth, bitGun.bmHeight, hMemDC, 0, 0, SRCCOPY);

            // 원과 총 이미지가 겹치면 파란색 테두리 그리기
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

        // Draw the message if needed
        if (showMessage) {
            message = L"더이상 나아갈수 없다.";
            SetTextColor(hDC, RGB(255, 0, 0));
            SetBkMode(hDC, TRANSPARENT);
            SIZE size;
            GetTextExtentPoint32(hDC, message, lstrlen(message), &size);
            TextOut(hDC, (rect.right - size.cx) / 2, rect.bottom - size.cy - 10, message, lstrlen(message));
        }

        // 추가 그리기 함수 호출
        DrawBall(hDC);

        if (showMessageBox) {
            DrawMessageBox(hDC, message);
        }

        if (enemyActive && currentBitmapIndex == 1) { // 적이 두 번째 맵에서만 나타나도록
            DrawEnemy(hDC, enemyX, enemyY, enemyWidth, enemyHeight); // 첫 번째 적 그리기
        }

        if (enemy2Active && currentBitmapIndex == 1) { // 두 번째 적이 두 번째 맵에서만 나타나도록
            DrawEnemy(hDC, enemy2X, enemy2Y, enemy2Width, enemy2Height); // 두 번째 적 그리기
        }

        // Draw player health
        DrawHealth(hDC);

        if (playerHealth <= 0) {
            DrawGameOver(hDC); // 게임 오버 메시지 그리기
        }

        DeleteDC(hMemDC);
        DeleteDC(hMemDC2);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_KEYDOWN:
        if (!gameOver) { // 게임 오버 상태가 아닐 때만 키 입력 처리
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
            if (CheckCollision(enemyX, enemyY, enemyWidth, enemyHeight) && enemyActive) {
                playerHealth--;
                enemyActive = false;
                KillTimer(hWnd, enemyTimerID);
                if (playerHealth <= 0) {
                    // 게임 오버 처리
                    gameOver = true; // 게임 오버 상태 설정
                    SetTimer(hWnd, gameOverTimerID, 3000, NULL); // 3초 후 게임 재시작
                }
                else {
                    // 5초 후에 적 재등장
                    SetTimer(hWnd, respawnTimerID, 5000, NULL);
                }
            }
            if (CheckCollision(enemy2X, enemy2Y, enemy2Width, enemy2Height) && enemy2Active) {
                playerHealth--;
                enemy2Active = false;
                KillTimer(hWnd, enemy2TimerID);
                if (playerHealth <= 0) {
                    // 게임 오버 처리
                    gameOver = true; // 게임 오버 상태 설정
                    SetTimer(hWnd, gameOverTimerID, 3000, NULL); // 3초 후 게임 재시작
                }
                else {
                    // 5초 후에 적 재등장
                    SetTimer(hWnd, respawnTimerID, 5000, NULL);
                }
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case WM_CHAR:
        if (!gameOver) { // 게임 오버 상태가 아닐 때만 문자 입력 처리
            switch (wParam) {
            case 'e':
            case 'E':
                showScanEllipse = true; // 스캔 원 활성화
                scan_ellipseX = ellipseX, scan_ellipseY = ellipseY;
                scan_ellipseX2 = scan_ellipseX + 300, scan_ellipseY2 = scan_ellipseY + 300;
                SetTimer(hWnd, Scan_ellipse, 50, NULL); // 타이머 시작
                break;

            case 'f':
            case 'F':
                // 비트맵 변경 및 메시지 처리
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
        }
        break;

    case WM_TIMER:
        // 스캔
        if (!gameOver && showScanEllipse) { // 게임 오버 상태가 아닐 때만 스캔 원 표시
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
        // 메시지
        if (wParam == messageTimerID) {
            showMessage = false;
            KillTimer(hWnd, messageTimerID);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        // 적
        if (!gameOver && wParam == enemyTimerID) { // 게임 오버 상태가 아닐 때만 첫 번째 적 타이머 동작
            int deltaX = ballX - enemyX;
            int deltaY = ballY - enemyY;

            if (abs(deltaX) > abs(deltaY)) {
                enemyX += (deltaX > 0) ? 5 : -5;
            }
            else {
                enemyY += (deltaY > 0) ? 5 : -5;
            }

            if (CheckCollision(enemyX, enemyY, enemyWidth, enemyHeight) && enemyActive) {
                playerHealth--;
                enemyActive = false;
                KillTimer(hWnd, enemyTimerID);
                if (playerHealth <= 0) {
                    // 게임 오버 처리
                    gameOver = true; // 게임 오버 상태 설정
                    SetTimer(hWnd, gameOverTimerID, 3000, NULL); // 3초 후 게임 재시작
                }
                else {
                    // 5초 후에 적 재등장
                    SetTimer(hWnd, respawnTimerID, 5000, NULL);
                }
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        if (!gameOver && wParam == enemy2TimerID) { // 게임 오버 상태가 아닐 때만 두 번째 적 타이머 동작
            int deltaX = ballX - enemy2X;
            int deltaY = ballY - enemy2Y;

            if (abs(deltaX) > abs(deltaY)) {
                enemy2X += (deltaX > 0) ? 3 : -3; // 더 느리게 이동
            }
            else {
                enemy2Y += (deltaY > 0) ? 3 : -3; // 더 느리게 이동
            }

            if (CheckCollision(enemy2X, enemy2Y, enemy2Width, enemy2Height) && enemy2Active) {
                playerHealth--;
                enemy2Active = false;
                KillTimer(hWnd, enemy2TimerID);
                if (playerHealth <= 0) {
                    // 게임 오버 처리
                    gameOver = true; // 게임 오버 상태 설정
                    SetTimer(hWnd, gameOverTimerID, 3000, NULL); // 3초 후 게임 재시작
                }
                else {
                    // 5초 후에 적 재등장
                    SetTimer(hWnd, respawnTimerID, 5000, NULL);
                }
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        if (!gameOver && wParam == respawnTimerID) { // 게임 오버 상태가 아닐 때만 적 재등장 타이머 동작
            KillTimer(hWnd, respawnTimerID);
            if (currentBitmapIndex == 1) { // 2번째 맵에서만 재등장
                enemyActive = true;
                enemyX = rect.right - enemyWidth; // 오른쪽에서부터 나타나도록 초기 위치 설정
                enemyY = ballY + rand() % 200 - 100; // 주인공의 Y 위치에 맞춰 초기화, 랜덤 오프셋 추가

                enemy2Active = true;
                enemy2X = rect.right - enemy2Width; // 오른쪽에서부터 나타나도록 초기 위치 설정
                enemy2Y = ballY + rand() % 200 - 100; // 주인공의 Y 위치에 맞춰 초기화, 랜덤 오프셋 추가

                SetTimer(hWnd, enemyTimerID, 50, NULL); // 첫 번째 적의 움직임을 위한 타이머 시작
                SetTimer(hWnd, enemy2TimerID, 50, NULL); // 두 번째 적의 움직임을 위한 타이머 시작
            }
        }
        if (wParam == gameOverTimerID) {
            KillTimer(hWnd, gameOverTimerID);
            gameOver = false; // 게임 오버 상태 해제
            ResetGame();
            InvalidateRect(hWnd, NULL, TRUE);
        }

        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    // 적 활성화 조건 체크 (2번째 맵에서만 활성화)
    if (!gameOver && currentBitmapIndex == 1 && bWidth2 > 800 && !enemyActive && !enemy2Active) { // 게임 오버 상태가 아닐 때만 적 활성화
        enemyActive = true;
        enemyX = rect.right - enemyWidth; // 오른쪽에서부터 나타나도록 초기 위치 설정
        enemyY = ballY + rand() % 200 - 100; // 주인공의 Y 위치에 맞춰 초기화, 랜덤 오프셋 추가

        enemy2Active = true;
        enemy2X = rect.right - enemy2Width; // 오른쪽에서부터 나타나도록 초기 위치 설정
        enemy2Y = ballY + rand() % 200 - 100; // 주인공의 Y 위치에 맞춰 초기화, 랜덤 오프셋 추가

        SetTimer(hWnd, enemyTimerID, 50, NULL); // 첫 번째 적의 움직임을 위한 타이머 시작
        SetTimer(hWnd, enemy2TimerID, 50, NULL); // 두 번째 적의 움직임을 위한 타이머 시작
    }
    // 적 비활성화 조건 체크 (다른 맵으로 이동할 때)
    if (currentBitmapIndex != 1) {
        if (enemyActive) {
            enemyActive = false;
            KillTimer(hWnd, enemyTimerID);
        }
        if (enemy2Active) {
            enemy2Active = false;
            KillTimer(hWnd, enemy2TimerID);
        }
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

    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // 하얀색 브러시
    FillRect(hdc, &messageBoxRect, hBrush);
    DeleteObject(hBrush);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0)); // 검정색 텍스트
    DrawText(hdc, message, -1, &messageBoxRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DrawEnemy(HDC hdc, int x, int y, int width, int height) {
    HBRUSH hBrush = CreateSolidBrush(RGB(139, 0, 0)); // 검붉은 색
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Rectangle(hdc, x, y, x + width, y + height);
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

void DrawGameOver(HDC hdc) {
    RECT gameOverRect;
    gameOverRect.left = 0;
    gameOverRect.top = 0;
    gameOverRect.right = rect.right;
    gameOverRect.bottom = rect.bottom;

    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0)); // 검정색 브러시
    FillRect(hdc, &gameOverRect, hBrush);
    DeleteObject(hBrush);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 0, 0)); // 빨간색 텍스트
    DrawText(hdc, L"GAME OVER", -1, &gameOverRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

bool CheckCollision(int enemyX, int enemyY, int enemyWidth, int enemyHeight) {
    return ballX2 > enemyX && ballX < enemyX + enemyWidth &&
        ballY2 > enemyY && ballY < enemyY + enemyHeight;
}

void DrawScanEllipse(HDC hdc)
{
    HRGN scanHRgn = CreateEllipticRgn(scan_ellipseX, scan_ellipseY, scan_ellipseX2, scan_ellipseY2);
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255)); // 흰색 브러시
    FrameRgn(hdc, scanHRgn, whiteBrush, 3, 3); // 외곽선을 그리기 위해 FrameRgn 사용
    DeleteObject(scanHRgn);
    DeleteObject(whiteBrush);
}

void ResetGame() {
    // 게임 초기 상태로 리셋
    playerHealth = 3;
    currentBitmapIndex = 0;
    ballX = 120; ballY = 390;
    ellipseX = 0; ellipseY = 270;
    bWidth = rect.left, bHeight = rect.top;
    bWidth2 = rect.right - rect.left, bHeight2 = rect.bottom - rect.top;
    enemyActive = false;
    enemyX = rect.right - enemyWidth;
    enemyY = ballY;
    enemy2Active = false;
    enemy2X = rect.right - enemy2Width;
    enemy2Y = ballY + 100;
}