// tfw10fcu.cpp
//
#include "stdafx.h"
#include "resource.h"
#include <Commctrl.h>
#include <wchar.h>
#include "Shlwapi.h"
#include "tfw10fcu.h"
#pragma comment(lib,"shlwapi.lib")
#define DllExport    __declspec( dllexport )

#pragma data_seg( ".ShareData" )
HHOOK g_hHookMouseWnd = NULL;
LONG_PTR	g_DefProc;
#pragma data_seg()
HINSTANCE g_hDll = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hDll = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if (nCode >= 0 || nCode != HC_NOREMOVE) {
		if (nCode == HC_ACTION){
			MOUSEHOOKSTRUCTEX *pmse = (MOUSEHOOKSTRUCTEX *)lParam;
			switch (wParam) {
				case WM_MOUSEMOVE:
					if (GetKeyState(VK_LBUTTON) < 0 || GetKeyState(VK_RBUTTON) < 0) {
						WCHAR pszClass[MAX_CLASS_NAME];
						if (GetClassName(pmse->hwnd, pszClass, MAX_CLASS_NAME)) {
							if (PathMatchSpec(pszClass, L"*listview*")) {
								RECT rc = {0, 0, 1, 1};
								RedrawWindow(GetParent(pmse->hwnd), &rc, 0, RDW_NOERASE | RDW_INVALIDATE);
							}
						}
					}
					break;
			}
		}
	}
	return CallNextHookEx(g_hHookMouseWnd, nCode, wParam, lParam);
}

DllExport HHOOK CALLBACK MouseHookInstall(void)
{
	if (g_hDll) {
		ChangeWindowMessageFilter(WM_MOUSEMOVE, MSGFLT_ADD);
		return g_hHookMouseWnd = SetWindowsHookEx(WH_MOUSE,(HOOKPROC)MouseProc, g_hDll, 0);
	}
		return NULL;
}

DllExport void CALLBACK MouseHookUninstall(void)
{
	if (g_hHookMouseWnd){
		UnhookWindowsHookEx(g_hHookMouseWnd);
	}
}
