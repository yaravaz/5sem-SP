#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <locale.h>

#define MAX_PROCESSES 10

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    HANDLE hWritePipe, hReadPipe;
    HANDLE  hMutex;
    HANDLE childProcesses[MAX_PROCESSES];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    if (argc < 4) {
        printf("usage: %s <process count> <lower bound> <upper bound> [mutex name]\n", argv[0]);
        return 1;
    }

    int processCount = atoi(argv[1]);
    int lowerBound = atoi(argv[2]);
    int upperBound = atoi(argv[3]);
    
    const char* defaultMutex = "Default";
    const char* mutexName = defaultMutex;

    wchar_t wideMutexName[256];

    if (argc > 4) {
        mutexName = argv[4];
        MultiByteToWideChar(CP_UTF8, 0, mutexName, -1, wideMutexName, sizeof(wideMutexName) / sizeof(wideMutexName[0]));
    }
    else {
        wchar_t envMutexName[32];
        DWORD result = GetEnvironmentVariable(L"MUTEX_NAME", envMutexName, sizeof(envMutexName) / sizeof(envMutexName[0]));

        if (result > 0 && result < sizeof(envMutexName)) {
            wcscpy(wideMutexName, envMutexName);
        }
    }

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        printf("CreatePipe failed.\n");
        return GetLastError();
    }

    hMutex = CreateMutex(NULL, FALSE, wideMutexName);
    if (hMutex == NULL)
    {
        printf("CreateMutex error");
        return GetLastError();
    }

    for (size_t i = 0; i < processCount; i++) {
        int genSize = (upperBound - lowerBound + 1) / processCount;
        int start = lowerBound + i * genSize;
        int end = (i == processCount - 1) ? upperBound : (start + genSize - 1);

        wchar_t command[128];

        wsprintf(command, L"D:\\лабы\\сп\\lab3\\lab3\\Laba3\\x64\\Debug\\Lab-03a-client.exe %d %d %s", start, end, wideMutexName);

        int process = 0;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.dwFlags = STARTF_USESTDHANDLES;
        si.cb = sizeof(STARTUPINFO);

        si.hStdInput = hReadPipe;
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;

        if (!CreateProcess(NULL, command, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            printf("CreateProcess error\n");
            return GetLastError();
        }

        childProcesses[process++] = pi.hProcess;
        CloseHandle(pi.hThread);
    }

    for (int i = 0; i < processCount;)
    {
        char buff[1024];
        DWORD dw;
        if (!ReadFile(hReadPipe, buff, sizeof(buff), &dw, NULL)) {
            printf("ReadPipe error");
            return GetLastError();
        }

        WaitForSingleObject(hMutex, INFINITE);

        buff[dw] = '\0';
        printf("%s\n\n", buff);
        i++;
        ReleaseMutex(hMutex);
    }

    for (int i = 0; i < processCount; i++) {
        CloseHandle(childProcesses[i]);
    }

    CloseHandle(hMutex);

    return 0;
}