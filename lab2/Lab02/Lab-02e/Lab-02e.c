#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void StartProcess(const wchar_t* processName, wchar_t* processArgs, PROCESS_INFORMATION* pi) {
    STARTUPINFO si = { sizeof(si) };
    ZeroMemory(pi, sizeof(PROCESS_INFORMATION));

    if (!CreateProcessW(processName, processArgs, NULL, NULL, FALSE, 0, NULL, NULL, &si, pi)) {
        fprintf(stderr, "Ошибка при создании процесса (%d).\n", GetLastError());
    }
    else {
        printf("Запущен процесс с PID: %lu\n", pi->dwProcessId);
    }
}

int main() {
    PROCESS_INFORMATION pi1, pi2;
    wchar_t* processName = L"Lab-02x.exe";

    wchar_t args1[] = L"Lab-02x.exe 5";
    StartProcess(processName, args1, &pi1);

    const char* envVarName = "ITER_NUM=10";
    if (!SetEnvironmentVariableA("ITER_NUM", "10")) {
        fprintf(stderr, "Ошибка установки переменной окружения (%d).\n", GetLastError());
    }

    wchar_t args2[] = L"Lab-02x.exe";
    StartProcess(processName, args2, &pi2);

    WaitForSingleObject(pi1.hProcess, INFINITE);
    WaitForSingleObject(pi2.hProcess, INFINITE);

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    return 0;
}