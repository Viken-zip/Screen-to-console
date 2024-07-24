#include <windows.h>
#include "Window.h"

LRESULT	Winproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_PAINT:
		{
			HDC hDc = GetDC(hWnd);
			RECT rect = {
				100,
				400,
				60,
				50,
			};
			HBRUSH hBr = CreateSolidBrush(RGB(255, 255, 255));
			FillRect(hDc, &rect, hBr);
			DeleteObject(hBr);
			ReleaseDC(hWnd, hDc);
			return 0;
		}
		case WM_DESTROY:
			PostQuitMessage(69); // you can add any number you want :D
			return 0;
		default:
			return DefWindowProcA(
				hWnd,
				uMsg,
				wParam,
				lParam
			);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE a, PSTR b, int nCmdShow) { //hPrevInstance lpCmdLine
	WNDCLASS class = {
		0,
		Winproc,
		0,
		0,
		hInstance,
		NULL,
		NULL,
		NULL,
		NULL,
		"FukWindowClass"
	};

	RegisterClassA(&class); // this fkr apparently takes one pointer to WNDCLASS and tadaa :|

	HWND windowHandle = CreateWindowA(
		"FukWindowClass",
		"Screen",
		WS_CAPTION | WS_POPUP | WS_SYSMENU, //apparently u use this "|" FUCKER so you can add more styles !!? why?
		50,
		50,
		640,
		480,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(windowHandle, nCmdShow);

	MSG msg;
	for (;;) {
		if (GetMessageA(&msg, NULL, 0, 0) == 0) {
			break;
		}
		DispatchMessageA(&msg);
	};

	return 0;
}