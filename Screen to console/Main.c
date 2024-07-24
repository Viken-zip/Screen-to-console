#include <windows.h>
#include <stdio.h>
#include "GetScreen.h" // this should be in the Window.c bc we dont need that here.
#include "Window.h"
#include "stdbool.h" // need bool header for bool's to work!? tf u on abt?

int main() {
	bool window = false;
	//bool liveScreen = true;
	
	/*if (liveScreen) {
		printf("take screen shots\n");
	}*/

	printf("lets run this screen shoter!");
	ScreenMain();

	if (window) {
		HINSTANCE hInstance = GetModuleHandle(NULL);
		int nCmdShow = SW_SHOWDEFAULT;
		WinMain(hInstance, NULL, NULL, nCmdShow);
	}
	
	printf("this fucker\n");
	return 0;
}