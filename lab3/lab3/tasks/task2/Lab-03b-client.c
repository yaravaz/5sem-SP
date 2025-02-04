#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#define BUFFER_SIZE 256
#define DEFAULT_PIPE_NAME "\\\\.\\pipe\\my_pipe"
#define DEFAULT_SEMAPHORE_NAME "MySemaphore"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    wchar_t buffer[BUFFER_SIZE];
    mbstowcs(buffer, argv[1], sizeof(argv[1]));

    char pipeName[256] = DEFAULT_PIPE_NAME;

    char env_pipe_name[128];
    DWORD pipe_result = GetEnvironmentVariableA("PIPE_NAME", env_pipe_name, sizeof(env_pipe_name));
    sprintf(pipeName, "\\\\.\\pipe\\%s", env_pipe_name);

    if (pipe_result == 0 || pipe_result >= sizeof(pipeName)) {
        strncpy(pipeName, DEFAULT_PIPE_NAME, sizeof(pipeName) - 1);
        pipeName[sizeof(pipeName) - 1] = '\0';
    }
    if (argc > 2) {
        sprintf(pipeName, "\\\\.\\pipe\\%s", argv[2]);
    }

    char semaphore_name[256] = DEFAULT_SEMAPHORE_NAME;
    char env_semaphore_name[128];
    DWORD sem_result = GetEnvironmentVariableA("MY_SEMAPHORE", env_semaphore_name, sizeof(env_semaphore_name));
    strncpy(semaphore_name, env_semaphore_name, sizeof(env_semaphore_name));

    if (sem_result == 0 || sem_result >= sizeof(pipeName)) {
        strncpy(semaphore_name, DEFAULT_SEMAPHORE_NAME, sizeof(semaphore_name) - 1);
        semaphore_name[sizeof(semaphore_name) - 1] = '\0';
    }
    if (argc > 3) {
        strncpy(semaphore_name, argv[3], sizeof(argv[3]));
    }

    wchar_t w_semaphore_name[256];
    mbstowcs(w_semaphore_name, semaphore_name, sizeof(semaphore_name));

    HANDLE hSemaphore = CreateSemaphore(NULL, 1, 1, w_semaphore_name);
    if (hSemaphore == NULL) {
        wprintf(L"Create semaphore: %d\n", GetLastError());
        return GetLastError();
    }

    HANDLE hPipe = CreateFileA(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        wprintf(L"Error connecting to pipe: %d\n", GetLastError());
        return 1;
    }

    DWORD bytesWritten, bytesRead;

    WaitForSingleObject(hSemaphore, INFINITE);

    Sleep(1000);

    if (!WriteFile(hPipe, buffer, sizeof(buffer), &bytesWritten, NULL)) {
        wprintf(L"Error writing to pipe: %d\n", GetLastError());
        CloseHandle(hPipe);
        return 1;
    }

    wchar_t input_buffer[BUFFER_SIZE];

    if (!ReadFile(hPipe, input_buffer, BUFFER_SIZE, &bytesRead, NULL)) {
        wprintf(L"Error reading from pipe: %d\n", GetLastError());
        CloseHandle(hPipe);
        return 1;
    }

    ReleaseSemaphore(hSemaphore, 1, NULL);

    wprintf(L"%s\n", input_buffer);

    CloseHandle(hPipe);
    CloseHandle(hSemaphore);

    system("pause");

    return 0;
}