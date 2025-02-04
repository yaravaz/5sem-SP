#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <wtypes.h>
#include <processthreadsapi.h>
#include <WinBase.h>
#include <windows.h>

void StartProcess(const wchar_t* processName) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessW(NULL, (LPSTR)processName, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("StartProcess failed (%d).\n", GetLastError());
    }
    else {
        printf("%ls\n", processName);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    SetCurrentDirectoryW(L"D:\\лабы\\сп\\lab2\\Lab02\\Debug\\Current\\");
    wchar_t name[] = L"Lab-02hw-0.exe";

    for (int i = 0; i < 5; i++) {
        name[9] = L'0' + (i + 1);
        StartProcess(name);
    }

    system("pause");

    return 0;
}
