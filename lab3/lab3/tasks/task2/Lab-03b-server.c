#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#define BUFFER_SIZE 256
#define DEFAULT_PIPE_NAME "\\\\.\\pipe\\my_pipe"

void to_upper_case(wchar_t* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = towupper(str[i]);
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    HANDLE hPipe;
    char pipeName[256] = DEFAULT_PIPE_NAME;
    wchar_t buffer[BUFFER_SIZE];
    DWORD bytesRead, bytesWritten;

    char env_pipe_name[128];
    DWORD result = GetEnvironmentVariableA("PIPE_NAME", env_pipe_name, sizeof(env_pipe_name));
    sprintf(pipeName, "\\\\.\\pipe\\%s", env_pipe_name);

    if (result == 0 || result >= sizeof(pipeName)) {
        strncpy(pipeName, DEFAULT_PIPE_NAME, sizeof(pipeName) - 1);
        pipeName[sizeof(pipeName) - 1] = '\0';
    }

    if (argc > 1) {
        sprintf(pipeName, "\\\\.\\pipe\\%s", argv[1]);
    }

    hPipe = CreateNamedPipeA(pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        wprintf(L"Error creating pipe: %d\n", GetLastError());
        return 1;
    }

    wprintf(L"Waiting for client connection...\n");

    while (1) {
        ConnectNamedPipe(hPipe, NULL);

        memset(buffer, 0, sizeof(buffer));
        if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
            wprintf(L"Error reading from pipe: %d\n", GetLastError());
            break;
        }

        wprintf(L"Received: %s\n", buffer);
        to_upper_case(buffer);
        wprintf(L"Sending back: %s\n", buffer);

        if (!WriteFile(hPipe, buffer, (wcslen(buffer) + 1) * sizeof(wchar_t), &bytesWritten, NULL)) {
            wprintf(L"Error writing to pipe: %d\n", GetLastError());
            break;
        }

        DisconnectNamedPipe(hPipe);

        wprintf(L"\n");
    }

    CloseHandle(hPipe);
}