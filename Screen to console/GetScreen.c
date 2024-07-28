#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "GetScreen.h"

typedef struct {
    int id;
    int width;
    int height;
    int rowSize;
    char* message;
    BYTE* pixels;
} ThreadData;

unsigned __stdcall threadBuildRow(void* parg) {
    ThreadData* data = (ThreadData*)parg;
    int length = data->width + 1;

    int bufferSize = (data->width * 24) + 1;
    data->message = (char*)malloc(bufferSize);
    if (data->message == NULL){ 
        perror("Failed to allocate memory for message in thread Function!");
        return 1;
    }

    char* rowBuffer = (char*)malloc(bufferSize);
    if (rowBuffer == NULL) {
        perror("Failed to allocate memory for rowBuffer in thread Function!");
        return 1;
    }
    rowBuffer[0] = '\0';

    BYTE* pixels = data->pixels;

    for (int i = 0; i < length-1; i++) { //i is att wich x is it is in this case
        int index = (data->height - data->id - 1) * data->rowSize + i * 4; // im going fkn insane
        BYTE blue = pixels[index];
        BYTE green = pixels[index + 1];
        BYTE red = pixels[index + 2];

        snprintf(rowBuffer + strlen(rowBuffer), bufferSize - strlen(rowBuffer), "\x1b[38;2;%d;%d;%dm#\x1b[0m", red, green, blue);
    }
    //data->message = rowBuffer; //fk strncpy_s
    strncpy_s(data->message, bufferSize, rowBuffer, bufferSize-1);
    data->message[bufferSize-1] = '\0';

    free(rowBuffer); //breaks the darn thing >:(
    return 0;
}

void clearScreen() {
    system("cls");
}

int getCpuThreadAmount() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}

void getConsoleDimensions(int* width, int* height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void getMonitorResolution(int* width, int* height) {
    //why dose this have a seperate function? ehh maybe somthing needs to bee added soon who knows
    *width = GetSystemMetrics(SM_CXSCREEN);
    *height = GetSystemMetrics(SM_CYSCREEN);
}

void AccessBitmapData(HBITMAP hBitmap) {
    BITMAP bmp;
    BITMAPINFO bi;
    HDC hdcMem = CreateCompatibleDC(NULL);

    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    // Set up the BITMAPINFO structure
    memset(&bi, 0, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = bmp.bmWidth;
    bi.bmiHeader.biHeight = bmp.bmHeight;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32; // 32-bit bitmap it is
    bi.bmiHeader.biCompression = BI_RGB;

    int rowSize = ((bmp.bmWidth * bi.bmiHeader.biBitCount + 31) / 32) * 4;
    int imageSize = rowSize * bmp.bmHeight;
    BYTE* pPixels = (BYTE*)malloc(imageSize);

    if (pPixels) {
        HDC hdcScreen = GetDC(NULL);
        GetDIBits(hdcScreen, hBitmap, 0, bmp.bmHeight, pPixels, &bi, DIB_RGB_COLORS);
        
        HANDLE* threads = malloc(bmp.bmHeight * sizeof(HANDLE));
        if (threads == NULL) {
            perror("Failed to allocate memory for threads!");
            free(pPixels);
            ReleaseDC(NULL, hdcScreen);
            return 1;
        }

        ThreadData* dataArray = malloc(bmp.bmHeight * sizeof(ThreadData));
        if (dataArray == NULL) {
            perror("Failed to allocate memory for dataArray!");
            free(threads);
            free(pPixels);
            ReleaseDC(NULL, hdcScreen);
            return 1;
        }

        for (int y = 0; y < bmp.bmHeight; ++y) {
            dataArray[y].id = y; //dont bother, the intellisense is a lie, it do the stoopid-
            dataArray[y].rowSize = rowSize;
            dataArray[y].width = bmp.bmWidth;
            dataArray[y].height = bmp.bmHeight;
            dataArray[y].pixels = pPixels;

            threads[y] = (HANDLE)_beginthreadex(
                NULL, 0, threadBuildRow, &dataArray[y], 0, NULL
            );
            if (threads[y] == NULL) {
                perror("Failed to create thread in thread loop!");
                // Clean up resources here if needed
            }
            Sleep(1L); //if not work for some reason try adding this back
        }

        WaitForMultipleObjects(bmp.bmHeight, threads, TRUE, INFINITE);

        clearScreen();

        // this could be made faster i bet
        for (int i = 0; i < bmp.bmHeight; i++) {
            if (i + 1 == bmp.bmHeight) {
                printf("%s", dataArray[i].message);
            }
            else {
                printf("%s\n", dataArray[i].message);
            }
            free(dataArray[i].message);
            CloseHandle(threads[i]);
        }

        free(dataArray);
        free(threads);
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

    // this is soo bad! we need to be able to check users primary monitor resolution!
    int width, height;
    getMonitorResolution(&width, &height);

    //make bitmap would maybe be better with png, but might slow the system down bc od convertion time
    HBITMAP hbDesktop = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(hDest, hbDesktop);

    BitBlt(hDest, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

    //scales bit map to fit console size
    int newWidth, newHeight;
    getConsoleDimensions(&newWidth, &newHeight);

    HBITMAP hbResized = CreateCompatibleBitmap(hdc, newWidth, newHeight);
    HDC hdcResized = CreateCompatibleDC(hdc);
    HBITMAP oldResizedBitmap = (HBITMAP)SelectObject(hdcResized, hbResized);

    StretchBlt(hdcResized, 0, 0, newWidth, newHeight, hDest, 0, 0, width, height, SRCCOPY);

    AccessBitmapData(hbResized); // hbDesktop, if want top use downscaled verion use: hbResized

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