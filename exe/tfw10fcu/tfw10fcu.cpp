// tfw10fcu.cpp : Defines the entry point for the application.

#include "stdafx.h"
#include "tfw10fcu.h"
#include <Shellapi.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib") 

#define MAX_LOADSTRING 100
#define TWM_TRAYICON	WM_APP

#ifdef _WIN64
#pragma comment(lib, "tfw10fcu64.lib")
#else
#pragma comment(lib, "tfw10fcu32.lib")
#endif
extern HHOOK CALLBACK MouseHookInstall(void);
extern void CALLBACK MouseHookUninstall(void);

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
BOOL bTrayIcon;
UINT uTaskBarRecreate = NULL;
HWND hwndMain;
HANDLE g_hMutex;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

void CreateTaskTray(BOOL f)
{
	if (f && bTrayIcon) {
		return;
	}
	bTrayIcon = TRUE;
	if (!uTaskBarRecreate) {
		uTaskBarRecreate = RegisterWindowMessage(L"TaskbarCreated");
	}
	NOTIFYICONDATA NotifyData;
	NotifyData.cbSize = sizeof(NotifyData);
	NotifyData.hWnd = hwndMain;
	NotifyData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	NotifyData.uCallbackMessage = TWM_TRAYICON;
	NotifyData.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
	lstrcpy(NotifyData.szTip, L"Tablacus Windows 10 Fall Creators Update(RS3) bug fix");
	
	for (int nDog = 5; Shell_NotifyIcon(NIM_ADD, &NotifyData) && --nDog > 0;) {
		Sleep(100);
	}
	DestroyIcon(NotifyData.hIcon);
}

void DeleteTaskTray()
{
	if (bTrayIcon) {
		bTrayIcon = FALSE;

		NOTIFYICONDATA NotifyData;
		NotifyData.cbSize = sizeof(NotifyData);
		NotifyData.hWnd = hwndMain;
		
		for (int nDog = 5; Shell_NotifyIcon(NIM_DELETE, &NotifyData) && --nDog > 0;) {
			Sleep(100);
		}
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TFW10FCU, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	//Multiple Launch
#ifdef _WIN64
	g_hMutex = CreateMutex(NULL, FALSE, L"tfw10fcu64");
#else
	g_hMutex = CreateMutex(NULL, FALSE, L"tfw10fcu32");
#endif
	if (WaitForSingleObject(g_hMutex, 0) != WAIT_TIMEOUT) {
		if (MouseHookInstall()) {
			CreateTaskTray(true);
			// Main message loop:
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			DeleteTaskTray();
			MouseHookUninstall();
		}
	}
	ReleaseMutex(g_hMutex);
	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BIG));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TFW10FCU);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hwndMain = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   return hwndMain != NULL;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		DeleteTaskTray();
		MouseHookUninstall();
		PostQuitMessage(0);
		break;

	case TWM_TRAYICON:
		switch (lParam) {
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case NIN_SELECT:
			case WM_CONTEXTMENU:
				SetForegroundWindow(hWnd);
				PostMessage(hWnd, WM_NULL, 0, 0);
				HMENU hMenu;
				hMenu= LoadMenu(hInst, MAKEINTRESOURCE(IDC_MENU));
				MENUITEMINFO mii;
				::ZeroMemory(&mii, sizeof(MENUITEMINFO));
				mii.cbSize = sizeof(MENUITEMINFO);
				mii.fMask  = MIIM_SUBMENU;
				GetMenuItemInfo(hMenu, 0, TRUE, &mii);
				POINT pt;
				GetCursorPos(&pt);
				TrackPopupMenuEx(mii.hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, hWnd, NULL);
				break;
			case WM_QUERYENDSESSION:
				return 1;
			default:
				if (lParam == uTaskBarRecreate) {
					CreateTaskTray(false);
				}
				break;
		}
		return 0;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
