#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


void StartProcess(const wchar_t* processName, PROCESS_INFORMATION* pi) {
    STARTUPINFO si = { sizeof(si) };
    ZeroMemory(pi, sizeof(PROCESS_INFORMATION));

    if (!CreateProcessW(processName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, pi)) {
        wprintf(L"Failed to start %s: %d\n", processName, GetLastError());
    }
    else {
        wprintf(L"Started process with PID: %lu\n", pi->dwProcessId);
    }
}

int main() {
    PROCESS_INFORMATION pi;
    const wchar_t* processName = L"D:\\лабы\\сп\\lab2\\Lab02\\Debug\\Lab-02inf.exe";

    StartProcess(processName, &pi);

    printf("Press any key to continue...\n");
    getchar();

    if (TerminateProcess(pi.hProcess, 0)) {
        wprintf(L"Terminated process with PID: %lu\n", pi.dwProcessId);
    }
    else {
        wprintf(L"Failed to terminate process: %d\n", GetLastError());
    }

    printf("Press any key to continue...\n");
    getchar();

    DWORD pid = GetProcessId(pi.hProcess);
    wprintf(L"PID of child process: %lu\n", pid);

    printf("Press any key to continue...\n");
    getchar();

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    pid = GetProcessId(pi.hProcess);
    wprintf(L"Trying to get PID after closing handle: %lu\n", pid);

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}