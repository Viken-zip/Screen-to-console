#include <windows.h>
#include <stdio.h>
#include "GetScreen.h" // this should be in the Window.c bc we dont need that here.
#include "Window.h"
#include "stdbool.h" // need bool header for bool's to work!? tf u on abt?
#include "TestThreads.h"

int main() {
	bool window = false;
	bool screenToConsole = true;
	bool testThreads = false;
	
	if (screenToConsole) {
		ScreenMain();
	}

	if (testThreads) {
		TestThreadsMain();
	}

	if (window) {
		HINSTANCE hInstance = GetModuleHandle(NULL);
		int nCmdShow = SW_SHOWDEFAULT;
		WinMain(hInstance, NULL, NULL, nCmdShow);
	}
	
	return 0;
}