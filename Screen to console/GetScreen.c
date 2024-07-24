#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "GetScreen.h"

void clearScreen() {
    system("cls");
}

int getCpuThreadAmount() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD numThreads = sysInfo.dwNumberOfProcessors;
    return numThreads;
}

void getConsoleDimensions(int* width, int* height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // Set the width and height values
    *width = columns;
    *height = rows;
}

void AccessBitmapData(HBITMAP hBitmap) {

    BITMAP bmp;
    BITMAPINFO bi;
    HDC hdcMem = CreateCompatibleDC(NULL);

    // Get bitmap details
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    // Set up the BITMAPINFO structure
    memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = bmp.bmWidth;
    bi.bmiHeader.biHeight = bmp.bmHeight;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32; // Assuming a 32-bit bitmap
    bi.bmiHeader.biCompression = BI_RGB;

    int rowSize = ((bmp.bmWidth * bi.bmiHeader.biBitCount + 31) / 32) * 4;
    int imageSize = rowSize * bmp.bmHeight;
    BYTE* pPixels = (BYTE*)malloc(imageSize);

    if (pPixels) {
        HDC hdcScreen = GetDC(NULL);
        GetDIBits(hdcScreen, hBitmap, 0, bmp.bmHeight, pPixels, &bi, DIB_RGB_COLORS);

        int frameBufferSize = ((bmp.bmWidth * bmp.bmHeight) * 24) + 1; //make buffet for the buffers :D
        char* frameBuffer = (char*)malloc(frameBufferSize);
        if (frameBuffer == NULL) {
            perror("Failed to allocate memory");
            //free(frameBuffer);
            exit(EXIT_FAILURE);
        }
        frameBuffer[0] = '\0';

        int bufferSize = (bmp.bmWidth * 24) + 1; // 7 characters per pixel wich i trought would be enough but noo, it needed 24 D:<
        char* rowBuffer = (char*)malloc(bufferSize);
        if (rowBuffer == NULL) {
            perror("Failed to allocate memory");
            //free(frameBuffer);
            exit(EXIT_FAILURE);
        }
        
        for (int y = 0; y < bmp.bmHeight; ++y) {

            rowBuffer[0] = '\0';

            for (int x = 0; x < bmp.bmWidth; ++x) {
                int index = (bmp.bmHeight - y - 1) * rowSize + x * 4;
                BYTE blue = pPixels[index];
                BYTE green = pPixels[index + 1];
                BYTE red = pPixels[index + 2];

                //printf("\x1b[38;2;%d;%d;%dm#\x1b[0m", red, green, blue); this m g
                snprintf(rowBuffer + strlen(rowBuffer), bufferSize - strlen(rowBuffer), "\x1b[38;2;%d;%d;%dm#\x1b[0m", red, green, blue);
            }

            //printf("%s\n", rowBuffer);
            if (y < bmp.bmHeight - 1) {
                snprintf(frameBuffer + strlen(frameBuffer), frameBufferSize - strlen(frameBuffer),
                    "%s\n", rowBuffer);
            }
            else {
                snprintf(frameBuffer + strlen(frameBuffer), frameBufferSize - strlen(frameBuffer),
                    "%s", rowBuffer);
            }
            //printf("\n");
        }
        clearScreen();
        printf("%s", frameBuffer);

        free(rowBuffer);
        free(frameBuffer);
        free(pPixels);
        ReleaseDC(NULL, hdcScreen);
    }

    DeleteDC(hdcMem);
}

void ClearBitmap(HBITMAP hBitmap) {
    DeleteObject(hBitmap);
}

void ScreenImage() 
{
    HDC hdc = GetDC(NULL);
    HDC hDest = CreateCompatibleDC(hdc);

    //int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    //int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = 1440;
    int width = 2560;

    //make bitmap would maybe be better with png, but might slow the system down bc od convertion time
    HBITMAP hbDesktop = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(hDest, hbDesktop);

    BitBlt(hDest, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

    /*
    this code bellow only makes the image to 640x480 for easier to show.
    */
    int newWidth, newHeight;
    //int newWidth = 120;
    //int newHeight = 30;

    getConsoleDimensions(&newWidth, &newHeight);

    HBITMAP hbResized = CreateCompatibleBitmap(hdc, newWidth, newHeight);
    HDC hdcResized = CreateCompatibleDC(hdc);
    HBITMAP oldResizedBitmap = (HBITMAP)SelectObject(hdcResized, hbResized);

    StretchBlt(hdcResized, 0, 0, newWidth, newHeight, hDest, 0, 0, width, height, SRCCOPY);
    /*
    the rest don't have anything with downscaling to do
    */
    AccessBitmapData(hbResized); // defualt hbDesktop, if want top use downscaled verion use: hbResized

    SelectObject(hDest, oldBitmap);
    ClearBitmap(hbDesktop);
    DeleteDC(hDest);
    ReleaseDC(NULL, hdc);

    ScreenImage();
}

int ScreenMain() {
    ScreenImage();
    return 0;
}