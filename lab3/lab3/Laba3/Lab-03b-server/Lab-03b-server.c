#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#define BUFFER_SIZE 256
#define DEFAULT_PIPE_NAME "\\\\.\\pipe\\Tube"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    char pipeName[256] = DEFAULT_PIPE_NAME;
    wchar_t buffer[BUFFER_SIZE];
    DWORD bytesRead, bytesWritten;

    if (argc > 1) {
        sprintf(pipeName, "\\\\.\\pipe\\%s", argv[1]);
    }
    else {
        char env_pipe_name[128];
        if (GetEnvironmentVariableA("PIPE_NAME", env_pipe_name, sizeof(env_pipe_name))) {
            sprintf(pipeName, "\\\\.\\pipe\\%s", env_pipe_name);
        }
    }

    HANDLE hPipe = CreateNamedPipeA(pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        wprintf(L"ErrorCreateNamedPipeA: %d\n", GetLastError());
        return 1;
    }

    wprintf(L"Listening...\n");

    while (1) {
        ConnectNamedPipe(hPipe, NULL);
        memset(buffer, 0, sizeof(buffer));

        if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
            wprintf(L"ErrorReadFile: %d\n", GetLastError());
            break;
        }

        wprintf(L"<-- %s\n", buffer);

        for (int i = 0; buffer[i]; i++) {
            buffer[i] = towupper(buffer[i]);
        }

        wprintf(L"%s -->\n", buffer);

        if (!WriteFile(hPipe, buffer, (wcslen(buffer) + 1) * sizeof(wchar_t), &bytesWritten, NULL)) {
            wprintf(L"ErrorWriteFile: %d\n", GetLastError());
            break;
        }

        DisconnectNamedPipe(hPipe);
        wprintf(L"\n");
    }

    CloseHandle(hPipe);
    return 0;
}