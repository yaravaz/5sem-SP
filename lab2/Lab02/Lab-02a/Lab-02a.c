#include <windows.h>
#include <stdio.h>

int main() {
    const wchar_t* fileName = L"D:\\лабы\\сп\\lab2\\Lab02\\Debug\\Lab-02x.exe";
    const wchar_t* iterationCount = L"5";

    PROCESS_INFORMATION processInfo[3];
    STARTUPINFO startupInfo[3];
    ZeroMemory(processInfo, sizeof(processInfo));
    ZeroMemory(startupInfo, sizeof(startupInfo));

    wchar_t commandLine[256];

    swprintf(commandLine, sizeof(commandLine) / sizeof(wchar_t), L"%s %s", fileName, iterationCount);

    if (!CreateProcess(commandLine, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo[0], &processInfo[0])) {
        DWORD error = GetLastError();
        wprintf(L"Error creating first process: %d\n", error);
    }
    
    swprintf(commandLine, sizeof(commandLine) / sizeof(wchar_t), L"%s %s", fileName, iterationCount);
    if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo[1], &processInfo[1])) {
        DWORD error = GetLastError();
        wprintf(L"Error creating second process: %d\n", error);
    }
    Sleep(2500);

    swprintf(commandLine, sizeof(commandLine) / sizeof(wchar_t), L"%s %s", fileName, iterationCount);
    if (!CreateProcess(fileName, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo[2], &processInfo[2])) {
        DWORD error = GetLastError();
        wprintf(L"Error creating third process: %d\n", error);
    }

    for (int i = 0; i < 3; i++) {
        if (processInfo[i].hProcess != NULL) {
            WaitForSingleObject(processInfo[i].hProcess, INFINITE);
            CloseHandle(processInfo[i].hProcess);
            CloseHandle(processInfo[i].hThread);
        }
    }

    return 0;
}