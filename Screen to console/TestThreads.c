#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <process.h>
#include "TestThreads.h"

#define BUF_SIZE 255

int getCpuThreads() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD numThreads = sysInfo.dwNumberOfProcessors;
    return numThreads;
}

typedef struct MyData {
    int id;
    char val[];
} MYDATA, *PMYDATA;

DWORD WINAPI threadFunc(LPVOID lpParam) {
    HANDLE hStdout;
    PMYDATA pDataArray;

    /* what the fk is going on here? */
    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    //cheking if there is a console, this is unessesary bc if it runs there is a console!
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE)
        return 1;

    pDataArray = (PMYDATA)lpParam;

    // this is supose to be thread safe? this dosent look safe at all to me!
    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d, %d\n"),
        pDataArray->id, pDataArray->val);
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);


    return 0;
}

typedef struct {
    int id;
    int width;
    char* message;
} ThreadData;

void miniFunc(void* parg) {
    printf_s("value: %d\n", *(int*)parg);
}

unsigned __stdcall miniFuncTwo(void* parg) {
    ThreadData* data = (ThreadData*)parg;
    int length = data->id + 1;
    data->message = malloc((length + 1) * sizeof(char));
    for (int i = 0; i < length; i++) {
        data->message[i] = '#';
    }
    data->message[length] = '\0';
    return 0;
}

int TestThreadsMain(){
    const int threadAmount = getCpuThreads();
    printf("Threads: %d\n", threadAmount);
    printf("America yaa :D\n");

    int param = 0;
    int BitMapHeight = 120;
    int bitMapWidth = 60;

    int frameBufferSize = ((bitMapWidth * BitMapHeight) * 24) + 1;
    char* frameBuffer = (char*)malloc(frameBufferSize);
    frameBuffer[0] = '\0';

    HANDLE *threads = malloc(BitMapHeight * sizeof(HANDLE));

    ThreadData* dataArray = malloc(BitMapHeight * sizeof(ThreadData));

    for (int i = 0; i < BitMapHeight; i++) {
        dataArray[i].id = i;
        dataArray[i].width = bitMapWidth;
        threads[i] = (HANDLE)_beginthreadex(
            NULL, 0, miniFuncTwo, &dataArray[i], 0, NULL
        );
        Sleep(1L);
    }

    WaitForMultipleObjects(BitMapHeight, threads, TRUE, INFINITE);


    for (int i = 0; i < BitMapHeight; i++) {
        printf("%s\n", dataArray[i].message);
        free(dataArray[i].message);
        CloseHandle(threads[i]);
    }

    free(dataArray);//fot the return string way 
    free(threads);
    return 0;
}