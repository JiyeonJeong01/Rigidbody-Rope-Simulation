#include "pch.h"
#include "framework.h"
#include "Simulation.h"
#include "MainApp.h"
#include "Utils.h"
#include <locale.h>

#define MAX_LOADSTRING 100

HINSTANCE g_hInst;                             
WCHAR szTitle[MAX_LOADSTRING];          
WCHAR szWindowClass[MAX_LOADSTRING];         
HWND    g_hWnd;
FILE* debug; 

// 프레임 관리
constexpr double FPS60_DT = 1.f / 60.f;
constexpr double FIXED_DT = 0.02f;
double fElapsedDT = 0.f;
double fFixedElapsedDT = 0.f;
double prevTime = GetTime();
double prevFixedTime = GetTime();

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SIMULATION, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMULATION));

	MSG msg;
	msg.message = WM_NULL;

	MainApp* pMainApp = MainApp::Create();

	if (nullptr == pMainApp)
		return FALSE;

	double fDeltaTime = 0.f;
	double fFixedTime = 0.f;

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				break;

			if (!hAccelTable || !TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			double currentTime = GetTime();
			double dt = double(currentTime - prevTime);
			prevTime = currentTime;
			prevFixedTime = currentTime;

			if (dt > 0.25f) dt = 0.25f;

			fElapsedDT += dt;
			fFixedElapsedDT += dt;

			while (fElapsedDT >= FPS60_DT)
			{
				pMainApp->Update_MainApp(FPS60_DT);
				pMainApp->LateUpdate_MainApp(FPS60_DT);
				pMainApp->Render_MainApp();

				fElapsedDT -= FPS60_DT;

				if (fFixedElapsedDT >= FIXED_DT)
				{
					pMainApp->Fixed_Update(FIXED_DT);
					fFixedElapsedDT -= FIXED_DT;
				}
			}

		}
	}

	Safe_Delete(pMainApp);

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMULATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance;

	RECT rc{ 0,0, WINCX, WINCY };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    HWND hWnd = CreateWindowW(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,              // 테두리 제거
        0, 0,
        screenW,
        screenH,
        nullptr, nullptr, hInstance, nullptr
    );

	if (!hWnd)
	{
		return FALSE;
	}

	g_hWnd = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
#pragma region Console
	case WM_CREATE:
	{
		AllocConsole();
		_tfreopen_s(&debug, _T("CONOUT$"), _T("w"), stdout);
		_tfreopen_s(&debug, _T("CONOUT$"), _T("r"), stdin);
		_tfreopen_s(&debug, _T("CONOUT$"), _T("w"), stderr);

		HWND hConsole = GetConsoleWindow();
		MoveWindow(hConsole, 0, 50, 600, 800, TRUE);
		MoveWindow(hWnd, 600, 50, WINCX, WINCY, TRUE);
	}
	break;

	case WM_CLOSE:
	{
		FreeConsole();
		DestroyWindow(hWnd);
	}
	break;
#pragma endregion
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(g_hWnd);
			break;
		}

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
