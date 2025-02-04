#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#define BUFFER_SIZE 256
#define DEFAULT_PIPE_NAME "\\\\.\\pipe\\MyPipe"
#define DEFAULT_SEMAPHORE_NAME "MySemaphore"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    wchar_t buffer[BUFFER_SIZE];
    size_t length = mbstowcs(buffer, argv[1], BUFFER_SIZE - 1);

    if (length != (size_t)-1) {
        buffer[length] = L'\0';
    }

    char pipeName[256];
    char semaphore_name[256];
    char envPipeName[128];
    char envSemaphoreName[128];

    if (GetEnvironmentVariableA("PIPE_NAME", envPipeName, sizeof(envPipeName))) {
        sprintf(pipeName, "\\\\.\\pipe\\%s", envPipeName);
    }
    else {
        strncpy(pipeName, DEFAULT_PIPE_NAME, sizeof(pipeName));
    }

    if (argc > 2) {
        sprintf(pipeName, "\\\\.\\pipe\\%s", argv[2]);
    }

    if (GetEnvironmentVariableA("SEMAPHORE_NAME", envSemaphoreName, sizeof(envSemaphoreName))) {
        strncpy(semaphore_name, envSemaphoreName, sizeof(semaphore_name));
    }
    else {
        strncpy(semaphore_name, DEFAULT_SEMAPHORE_NAME, sizeof(semaphore_name));
    }

    if (argc > 3) {
        strncpy(semaphore_name, argv[3], sizeof(semaphore_name));
    }

    wchar_t w_semaphore_name[256];
    mbstowcs(w_semaphore_name, semaphore_name, sizeof(w_semaphore_name) / sizeof(wchar_t));

    HANDLE hSemaphore = CreateSemaphore(NULL, 1, 1, w_semaphore_name);
    if (hSemaphore == NULL) {
        wprintf(L"Create semaphore: %d\n", GetLastError());
        return GetLastError();
    }

    HANDLE hPipe = CreateFileA(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        wprintf(L"ErrorCreateFileA: %d\n", GetLastError());
        return 1;
    }

    DWORD bytesWritten, bytesRead;
    WaitForSingleObject(hSemaphore, INFINITE);
    Sleep(5000);

    if (!WriteFile(hPipe, buffer, (wcslen(buffer) + 1) * sizeof(wchar_t), &bytesWritten, NULL)) {
        wprintf(L"ErrorWriteFile: %d\n", GetLastError());
        CloseHandle(hPipe);
        return 1;
    }

    wchar_t input_buffer[BUFFER_SIZE];
    if (!ReadFile(hPipe, input_buffer, sizeof(input_buffer), &bytesRead, NULL)) {
        wprintf(L"ErrorReadFile: %d\n", GetLastError());
        CloseHandle(hPipe);
        return 1;
    }

    ReleaseSemaphore(hSemaphore, 1, NULL);
    wprintf(L"%s\n", input_buffer);

    CloseHandle(hPipe);
    CloseHandle(hSemaphore);
    return 0;
}