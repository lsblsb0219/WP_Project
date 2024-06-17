#include <windows.h>
#include <windowsx.h> // �߰��� ��� ����
#include "resource.h"
#include "fmod.hpp"
#include "fmod_errors.h"

#pragma comment (lib, "fmod_vc.lib")

FMOD::System* ssystem;
FMOD_RESULT result;
FMOD::Sound* rain_sound;
void* extradriverdata = 0;
FMOD::Channel* channel = 0;

RECT rect;

int ballX, ballY, ballX2, ballY2, ellipseX, ellipseY, ellipseX2, ellipseY2, scan_ellipseX2, scan_ellipseX, scan_ellipseY2, scan_ellipseY;
int see_bw, see_bh, bWidth, bHeight, bWidth2, bHeight2;
int currentBitmapIndex = 0;
bool showMessageBox;
bool showMessage = false; // �޽����� ǥ������ ����
LPCWSTR message;
UINT_PTR messageTimerID = 1; // Ÿ�̸� ID

// ��ĵ
bool showScanEllipse = false; // ��ĵ ���� ǥ������ ����
UINT_PTR Scan_ellipse = 2;
int size_cheack; // ��ĵ �� ũ�� ���� Ȯ��


UINT_PTR respawnTimerID = 3; // �� ������� ���� Ÿ�̸� ID
UINT_PTR gameOverTimerID = 4; // ���� ���� �� ������� ���� Ÿ�̸� ID

// ���ΰ� ü�� ����
int playerHealth = 3; // �ʱ� ü�� 3

// �� ���� ����
bool enemyActive = false;
int enemyX, enemyY, enemyWidth, enemyHeight;
UINT_PTR enemyTimerID = 2;

// �� ��° �� ���� ����
bool enemy2Active = false;
int enemy2X, enemy2Y, enemy2Width, enemy2Height;
UINT_PTR enemy2TimerID = 5;

// �� ��° ���� �� ���� ����
int enemyCount = 0;
int maxEnemies = 20;
int removedEnemies = 0;
int activeEnemies = 0; // ���� Ȱ��ȭ�� ���� ��
bool enemiesActive[20] = { false };
int enemiesX[20], enemiesY[20];
UINT_PTR enemyTimers[20];

// ���� �޽��� ���� ����
bool showDirectionMessage = true;
bool showScanMessage = false;
bool showInteractionMessage = false;
UINT_PTR instructionTimerID = 6;

void DrawBall(HDC hdc);
void DrawMessageBox(HDC hdc, LPCWSTR message);
void DrawScanEllipse(HDC hdc);
void DrawEnemy(HDC hdc, int x, int y, int width, int height);
void DrawHealth(HDC hdc);
void DrawGameOver(HDC hdc);
bool CheckCollision(int enemyX, int enemyY, int enemyWidth, int enemyHeight);
void ResetGame();
void SpawnEnemy(HWND hWnd);
void ShowMessage(HDC hdc, LPCWSTR message);

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

	static bool gameOver = false; // ���� ���� ���� ���� �߰�

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		result = FMOD::System_Create(&ssystem);

		if (result != FMOD_OK)
			exit(0);
		ssystem->init(32, FMOD_INIT_NORMAL, extradriverdata); //--- ���� �ý��� �ʱ�ȭ
		ssystem->createSound("rain.mp3", FMOD_LOOP_NORMAL, 0, &rain_sound); //--- 1�� ���� ���� �� ����

		channel->stop();
		ssystem->playSound(rain_sound, 0, false, &channel); //--- 1�� ���� ���

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

		// ���� �ʱ� ��ġ�� ũ�� ����
		enemyWidth = 60;
		enemyHeight = 60;
		enemyX = rect.right - enemyWidth;
		enemyY = ballY; // ���ΰ��� Y ��ġ�� ���� �ʱ�ȭ

		// �� ��° �� �ʱ� ��ġ�� ũ�� ����
		enemy2Width = 60;
		enemy2Height = 60;
		enemy2X = rect.right - enemy2Width;
		enemy2Y = ballY + 100; // ���ΰ��� Y ��ġ�� ���� �ʱ�ȭ

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

		// �޽��� ���
		if (showMessage) {
			message = L"���̻� ���ư��� ����.";
			SetTextColor(hDC, RGB(255, 0, 0));
			SetBkMode(hDC, TRANSPARENT);
			SIZE size;
			GetTextExtentPoint32(hDC, message, lstrlen(message), &size);
			TextOut(hDC, (rect.right - size.cx) / 2, rect.bottom - size.cy - 10, message, lstrlen(message));
		}

		// �߰� �׸��� �Լ� ȣ��
		DrawBall(hDC);

		if (showMessageBox) {
			DrawMessageBox(hDC, message);
		}

		if (enemyActive && currentBitmapIndex == 1) { // ���� �� ��° �ʿ����� ��Ÿ������
			DrawEnemy(hDC, enemyX, enemyY, enemyWidth, enemyHeight); // ù ��° �� �׸���
		}

		if (enemy2Active && currentBitmapIndex == 1) { // �� ��° ���� �� ��° �ʿ����� ��Ÿ������
			DrawEnemy(hDC, enemy2X, enemy2Y, enemy2Width, enemy2Height); // �� ��° �� �׸���
		}

		if (currentBitmapIndex == 2) { // �� ��° �ʿ����� �� �׸���
			for (int i = 0; i < maxEnemies; i++) {
				if (enemiesActive[i]) {
					DrawEnemy(hDC, enemiesX[i], enemiesY[i], enemyWidth, enemyHeight);
				}
			}
		}

		// Draw player health
		DrawHealth(hDC);

		if (playerHealth <= 0) {
			DrawGameOver(hDC); // ���� ���� �޽��� �׸���
		}

		// ���� �޽��� �׸���
		if (showDirectionMessage) {
			ShowMessage(hDC, L"���� ���� - WASD");
		}
		else if (showScanMessage) {
			ShowMessage(hDC, L"��ĵ - E");
		}
		else if (showInteractionMessage) {
			ShowMessage(hDC, L"��ȣ�ۿ� - F");
		}

		DeleteDC(hMemDC);
		DeleteDC(hMemDC2);

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_KEYDOWN:
		if (!gameOver) { // ���� ���� ���°� �ƴ� ���� Ű �Է� ó��
			switch (wParam) {
			case 'A': // ����
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
			case 'D': // ������
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
				// ���� �޽��� ����
				if (showDirectionMessage) {
					showDirectionMessage = false;
					showScanMessage = true;
					SetTimer(hWnd, instructionTimerID, 5000, NULL);
				}
				break;
			case 'W': // ����
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
			case 'S': // �Ʒ���
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
					// ���� ���� ó��
					gameOver = true; // ���� ���� ���� ����
					SetTimer(hWnd, gameOverTimerID, 3000, NULL); // 3�� �� ���� �����
				}
				else {
					// 5�� �Ŀ� �� �����
					SetTimer(hWnd, respawnTimerID, 5000, NULL);
				}
			}
			if (CheckCollision(enemy2X, enemy2Y, enemy2Width, enemy2Height) && enemy2Active) {
				playerHealth--;
				enemy2Active = false;
				KillTimer(hWnd, enemy2TimerID);
				if (playerHealth <= 0) {
					// ���� ���� ó��
					gameOver = true; // ���� ���� ���� ����
					SetTimer(hWnd, gameOverTimerID, 3000, NULL); // 3�� �� ���� �����
				}
				else {
					// 5�� �Ŀ� �� �����
					SetTimer(hWnd, respawnTimerID, 5000, NULL);
				}
			}
			if (currentBitmapIndex == 2 && activeEnemies < 5) {
				SpawnEnemy(hWnd); // �� ���� �Լ� ȣ��
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_LBUTTONDOWN:
		if (currentBitmapIndex == 2) { // �� ��° �ʿ����� ���� Ŭ���Ͽ� ���� ����
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			for (int i = 0; i < maxEnemies; i++) {
				if (enemiesActive[i] && xPos >= enemiesX[i] && xPos <= enemiesX[i] + enemyWidth &&
					yPos >= enemiesY[i] && yPos <= enemiesY[i] + enemyHeight) {
					enemiesActive[i] = false;
					removedEnemies++;
					activeEnemies--;
					if (removedEnemies >= 10) {
						for (int j = 0; j < maxEnemies; j++) {
							KillTimer(hWnd, enemyTimers[j]);
						}
						showMessage = true;
						SetTimer(hWnd, messageTimerID, 3000, NULL);
					}
					InvalidateRect(hWnd, NULL, TRUE);
					break;
				}
			}
		}
		break;

	case WM_CHAR:
		if (!gameOver) { // ���� ���� ���°� �ƴ� ���� ���� �Է� ó��
			switch (wParam) {
			case 'e':
			case 'E':
				showScanEllipse = true; // ��ĵ �� Ȱ��ȭ
				scan_ellipseX = ellipseX, scan_ellipseY = ellipseY;
				scan_ellipseX2 = scan_ellipseX + 300, scan_ellipseY2 = scan_ellipseY + 300;
				SetTimer(hWnd, Scan_ellipse, 50, NULL); // Ÿ�̸� ����

				// ���� �޽��� ����
				if (showScanMessage) {
					showScanMessage = false;
					showInteractionMessage = true;
					SetTimer(hWnd, instructionTimerID, 5000, NULL);
				}
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

					if (currentBitmapIndex == 2) {
						enemyCount = 0;
						removedEnemies = 0;
						activeEnemies = 0;
						for (int i = 0; i < maxEnemies; i++) {
							enemiesActive[i] = false;
						}
						SpawnEnemy(hWnd);
					}

					InvalidateRect(hWnd, NULL, TRUE);
				}
				else if (ballX2 >= rect.right && currentBitmapIndex == 3) {
					showMessage = true;
					SetTimer(hWnd, messageTimerID, 3000, NULL);
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
		}
		break;

	case WM_TIMER:
		// ��ĵ
		if (!gameOver && showScanEllipse) { // ���� ���� ���°� �ƴ� ���� ��ĵ �� ǥ��
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
		// �޽���
		if (wParam == messageTimerID) {
			showMessage = false;
			KillTimer(hWnd, messageTimerID);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		// ��
		if (!gameOver && wParam == enemyTimerID) { // ���� ���� ���°� �ƴ� ���� ù ��° �� Ÿ�̸� ����
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
					// ���� ���� ó��
					gameOver = true; // ���� ���� ���� ����
					SetTimer(hWnd, gameOverTimerID, 3000, NULL); // 3�� �� ���� �����
				}
				else {
					// 5�� �Ŀ� �� �����
					SetTimer(hWnd, respawnTimerID, 5000, NULL);
				}
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (!gameOver && wParam == enemy2TimerID) { // ���� ���� ���°� �ƴ� ���� �� ��° �� Ÿ�̸� ����
			int deltaX = ballX - enemy2X;
			int deltaY = ballY - enemy2Y;

			if (abs(deltaX) > abs(deltaY)) {
				enemy2X += (deltaX > 0) ? 3 : -3; // �� ������ �̵�
			}
			else {
				enemy2Y += (deltaY > 0) ? 3 : -3; // �� ������ �̵�
			}

			if (CheckCollision(enemy2X, enemy2Y, enemy2Width, enemy2Height) && enemy2Active) {
				playerHealth--;
				enemy2Active = false;
				KillTimer(hWnd, enemy2TimerID);
				if (playerHealth <= 0) {
					// ���� ���� ó��
					gameOver = true; // ���� ���� ���� ����
					SetTimer(hWnd, gameOverTimerID, 3000, NULL); // 3�� �� ���� �����
				}
				else {
					// 5�� �Ŀ� �� �����
					SetTimer(hWnd, respawnTimerID, 5000, NULL);
				}
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (!gameOver && wParam == respawnTimerID) { // ���� ���� ���°� �ƴ� ���� �� ����� Ÿ�̸� ����
			KillTimer(hWnd, respawnTimerID);
			if (currentBitmapIndex == 1) { // 2��° �ʿ����� �����
				enemyActive = true;
				enemyX = rect.right - enemyWidth; // �����ʿ������� ��Ÿ������ �ʱ� ��ġ ����
				enemyY = ballY + rand() % 200 - 100; // ���ΰ��� Y ��ġ�� ���� �ʱ�ȭ, ���� ������ �߰�

				enemy2Active = true;
				enemy2X = rect.right - enemy2Width; // �����ʿ������� ��Ÿ������ �ʱ� ��ġ ����
				enemy2Y = ballY + rand() % 200 - 100; // ���ΰ��� Y ��ġ�� ���� �ʱ�ȭ, ���� ������ �߰�

				SetTimer(hWnd, enemyTimerID, 50, NULL); // ù ��° ���� �������� ���� Ÿ�̸� ����
				SetTimer(hWnd, enemy2TimerID, 50, NULL); // �� ��° ���� �������� ���� Ÿ�̸� ����
			}
		}
		if (wParam == gameOverTimerID) {
			KillTimer(hWnd, gameOverTimerID);
			gameOver = false; // ���� ���� ���� ����
			ResetGame();
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam >= 100 && wParam < 120) { // �� ��° ���� �� Ÿ�̸�
			int index = wParam - 100;
			enemiesX[index] -= 20;
			if (enemiesX[index] < 0) {
				enemiesActive[index] = false;
				activeEnemies--;
				KillTimer(hWnd, enemyTimers[index]);
				if (removedEnemies < 10) { // 10������ ���� ������ ���� ��� ���� ����
					gameOver = true; // ���� ���� ���� ����
					SetTimer(hWnd, gameOverTimerID, 3000, NULL); // 3�� �� ���� �����
				}
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		// ���� �޽��� Ÿ�̸�
		if (wParam == instructionTimerID) {
			KillTimer(hWnd, instructionTimerID);
			if (showScanMessage) {
				showScanMessage = false;
				showInteractionMessage = true;
				SetTimer(hWnd, instructionTimerID, 5000, NULL);
			}
			else if (showInteractionMessage) {
				showInteractionMessage = false;
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	// �� Ȱ��ȭ ���� üũ (2��° �ʿ����� Ȱ��ȭ)
	if (!gameOver && currentBitmapIndex == 1 && bWidth2 > 800 && !enemyActive && !enemy2Active) { // ���� ���� ���°� �ƴ� ���� �� Ȱ��ȭ
		enemyActive = true;
		enemyX = rect.right - enemyWidth; // �����ʿ������� ��Ÿ������ �ʱ� ��ġ ����
		enemyY = ballY + rand() % 200 - 100; // ���ΰ��� Y ��ġ�� ���� �ʱ�ȭ, ���� ������ �߰�

		enemy2Active = true;
		enemy2X = rect.right - enemy2Width; // �����ʿ������� ��Ÿ������ �ʱ� ��ġ ����
		enemy2Y = ballY + rand() % 200 - 100; // ���ΰ��� Y ��ġ�� ���� �ʱ�ȭ, ���� ������ �߰�

		SetTimer(hWnd, enemyTimerID, 50, NULL); // ù ��° ���� �������� ���� Ÿ�̸� ����
		SetTimer(hWnd, enemy2TimerID, 50, NULL); // �� ��° ���� �������� ���� Ÿ�̸� ����
	}
	// �� ��Ȱ��ȭ ���� üũ (�ٸ� ������ �̵��� ��)
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

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // �Ͼ�� �귯��
	FillRect(hdc, &messageBoxRect, hBrush);
	DeleteObject(hBrush);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0, 0, 0)); // ������ �ؽ�Ʈ
	DrawText(hdc, message, -1, &messageBoxRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DrawEnemy(HDC hdc, int x, int y, int width, int height) {
	HBRUSH hBrush = CreateSolidBrush(RGB(139, 0, 0)); // �˺��� ��
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Rectangle(hdc, x, y, x + width, y + height);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
}

void DrawHealth(HDC hdc) {
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // ������ �귯��
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

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0)); // ������ �귯��
	FillRect(hdc, &gameOverRect, hBrush);
	DeleteObject(hBrush);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 0, 0)); // ������ �ؽ�Ʈ
	DrawText(hdc, L"GAME OVER", -1, &gameOverRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

bool CheckCollision(int enemyX, int enemyY, int enemyWidth, int enemyHeight) {
	return ballX2 > enemyX && ballX < enemyX + enemyWidth &&
		ballY2 > enemyY && ballY < enemyY + enemyHeight;
}

void DrawScanEllipse(HDC hdc)
{
	HRGN scanHRgn = CreateEllipticRgn(scan_ellipseX, scan_ellipseY, scan_ellipseX2, scan_ellipseY2);
	HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255)); // ��� �귯��
	FrameRgn(hdc, scanHRgn, whiteBrush, 3, 3); // �ܰ����� �׸��� ���� FrameRgn ���
	DeleteObject(scanHRgn);
	DeleteObject(whiteBrush);
}

void ResetGame() {
	// ���� �ʱ� ���·� ����
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

void SpawnEnemy(HWND hWnd) {
	if (enemyCount < maxEnemies && activeEnemies < 5) {
		int index = enemyCount++;
		enemiesX[index] = rect.right - enemyWidth;
		enemiesY[index] = rect.bottom - enemyHeight - (rand() % 100);
		enemiesActive[index] = true;
		enemyTimers[index] = SetTimer(hWnd, 100 + index, 50, NULL);
		activeEnemies++;
	}
}

void ShowMessage(HDC hdc, LPCWSTR message) {
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);
	SIZE size;
	GetTextExtentPoint32(hdc, message, lstrlen(message), &size);
	TextOut(hdc, (rect.right - size.cx) / 2, rect.bottom - size.cy - 10, message, lstrlen(message));
}
