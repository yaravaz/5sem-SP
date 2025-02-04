#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "Shlwapi.lib")

#define BUFFER_SIZE 1024

HANDLE hFile = NULL;

int OpenFiles(const char* fileName);
BOOL AddRow(HANDLE hFile, const wchar_t* str, int pos);
BOOL RemRow(HANDLE hFile, int pos);
BOOL PrintRow(HANDLE hFile, int pos);
BOOL PrintRows(HANDLE hFile);
void CloseFile(HANDLE hFile);

int main(int argc, char* argv[]) {

    wchar_t row[BUFFER_SIZE];
    int pos;
    char option;

    if (argc < 2) {
        printf("usage: %s <file path>\n", argv[0]);
        return 1;
    }

    const char* path = argv[1];

    while (1) {
        printf("Select operation:\n1. Open file.\n2. Insert line.\n3. Delete line.\n4. Print line.\n5. Print file.\n6. Close file.\n0. Exit.\n");
        option = getchar();
        getchar();

        switch (option) {
        case '1':
            OpenFiles(path);
            break;
        case '2':
            printf("Enter line: ");
            fgetws(row, BUFFER_SIZE, stdin);
            size_t len = wcslen(row);
            if (len > 0 && row[len - 1] == '\n') {
                row[len - 1] = '\0';
            }
            if (wcslen(row) == 0) {
                printf("Error: line cannot be empty.\n");
                return 1;
            }
            printf("Enter position: ");
            scanf("%d", &pos);
            getchar();
            if (pos < -1) {
                printf("Error: invalid position.\n");
                return 1;
            }
            AddRow(hFile, row, pos);
            break;
        case '3':
            printf("Enter position: ");
            scanf("%d", &pos);
            getchar();
            if (pos < -1) {
                printf("Error: invalid position.\n");
                return 1;
            }
            RemRow(hFile, pos);
            break;
        case '4':
            printf("Enter position: ");
            scanf("%d", &pos);
            getchar();
            if (pos < -1) {
                printf("Error: invalid position.\n");
                return 1;
            }
            PrintRow(hFile, pos);
            break;
        case '5':
            PrintRows(hFile);
            break;
        case '6':
            CloseFile(hFile);
            break;
        case '0':
            return 0;
        }
    }

    return 0;
}

int OpenFiles(const char* fileName) {
    hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Error opening file: %s\n", fileName);
        return GetLastError();
    }
    printf("File '%s' successfully opened.\n", fileName);
    return 0;
}

BOOL AddRow(HANDLE hFile, const wchar_t* str, int pos) {
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        wprintf(L"Error GetFileSizeEx\n");
        return FALSE;
    }

    int strLength = wcslen(str);
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, str, strLength, NULL, 0, NULL, NULL);
    char* insertStr = (char*)malloc(bufferSize + 1);
    WideCharToMultiByte(CP_UTF8, 0, str, strLength, insertStr, bufferSize, NULL, NULL);
    insertStr[bufferSize] = '\0';

    char* buf = (char*)malloc(fileSize.QuadPart + 1);
    DWORD bytesRead;
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    if (!ReadFile(hFile, buf, fileSize.QuadPart, &bytesRead, NULL)) {
        free(insertStr);
        free(buf);
        CloseHandle(hFile);
        wprintf(L"Error ReadFile\n");
        return FALSE;
    }
    buf[fileSize.QuadPart] = '\0';

    char* result = (char*)malloc(fileSize.QuadPart + strlen(insertStr) + 3);
    result[0] = '\0';
    size_t position = 0;
    int rowCount = 1;

    while (position < fileSize.QuadPart) {
        if (rowCount == pos) {
            strcat(result, insertStr);
            strcat(result, "\n");
        }

        while (position < fileSize.QuadPart && buf[position] != '\n') {
            strncat(result, &buf[position], 1);
            position++;
        }
        if (position < fileSize.QuadPart) {
            strncat(result, "\n", 1);
            position++;
        }
        rowCount++;
    }

    if (pos == -1) {
        strcat(result, insertStr);
        strcat(result, "\n");
    }

    if (pos == 0) {
        char* temp = (char*)malloc(strlen(result) + strlen(insertStr) + 2);
        sprintf(temp, "%s\n%s", insertStr, result);
        free(result);
        result = temp;
    }

    SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    DWORD bytesWritten;
    if (!WriteFile(hFile, result, strlen(result), &bytesWritten, NULL) ||
        !SetEndOfFile(hFile)) {
        free(insertStr);
        free(buf);
        free(result);
        CloseHandle(hFile);
        wprintf(L"Error WriteFile/SetEndOfFile\n");
        return FALSE;
    }

    free(insertStr);
    free(buf);
    free(result);
    return TRUE;
}

int CountRows(HANDLE hFile, char* ptr) {

    int rowCount = 0;
    while (*ptr) {
        if (*ptr == '\n') {
            rowCount++;
        }
        ptr++;
    }
    SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    return rowCount;
}

BOOL RemRow(HANDLE hFile, int pos) {
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        wprintf(L"Error GetFileSizeEx\n");
        return FALSE;
    }

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    char* buf = (char*)malloc(fileSize.QuadPart + 1);
    DWORD bytesRead;
    if (!ReadFile(hFile, buf, fileSize.QuadPart, &bytesRead, NULL)) {
        free(buf);
        CloseHandle(hFile);
        wprintf(L"Error ReadFile\n");
        return FALSE;
    }
    buf[fileSize.QuadPart] = '\0';

    char* result = (char*)malloc(fileSize.QuadPart + 1);
    result[0] = '\0';
    int rowCount = 1;
    char* ptr = buf;
    char* start = buf;
    int rows = CountRows(hFile, ptr);

    while (*ptr) {
        if (rowCount == pos) {
            while (*ptr && *ptr != '\n') {
                ptr++;
            }
            if (*ptr == '\n') ptr++;
            rowCount++;
        }
        else if (pos == 0) {
            while (*ptr && *ptr != '\n') {
                ptr++;
            }
            if (*ptr == '\n') ptr++;
            strcat(result, ptr);
            break;
        }
        else if (pos == -1) {
            while (*ptr && *ptr != '\n' && rows != rowCount) {
                strncat(result, ptr++, 1);
            }
            strcat(result, "\n");
            if (*ptr == '\n') ptr++;
            rowCount++;
            if (rows == rowCount) break;
        }
        else {
            while (*ptr && *ptr != '\n') {
                strncat(result, ptr, 1);
                ptr++;
            }
            if (*ptr == '\n') {
                strncat(result, "\n", 1);
                ptr++;
            }
            rowCount++;
        }
    }

    SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    DWORD bytesWritten;
    if (!WriteFile(hFile, result, strlen(result), &bytesWritten, NULL) ||
        !SetEndOfFile(hFile)) {
        free(buf);
        free(result);
        CloseHandle(hFile);
        wprintf(L"Error WriteFile/SetEndOfFile\n");
        return FALSE;
    }

    free(buf);
    free(result);
    return TRUE;
}

BOOL PrintRow(HANDLE hFile, int pos) {
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        wprintf(L"Error GetFileSizeEx\n");
        return FALSE;
    }

    char* buf = (char*)malloc(fileSize.QuadPart + 1);
    DWORD bytesRead;
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    if (!ReadFile(hFile, buf, fileSize.QuadPart, &bytesRead, NULL)) {
        free(buf);
        CloseHandle(hFile);
        wprintf(L"Error ReadFile\n");
        return FALSE;
    }
    buf[fileSize.QuadPart] = '\0';

    char* ptr = buf;
    int rowCount = 1;

    while (*ptr) {
        if (rowCount == pos) {
            while (*ptr && *ptr != '\n') {
                putchar(*ptr++);
            }
            putchar('\n');
            break;
        }
        else {
            while (*ptr && *ptr != '\n') {
                ptr++;
            }
            if (*ptr == '\n') ptr++;
            rowCount++;
        }
    }

    free(buf);
    return TRUE;
}

BOOL PrintRows(HANDLE hFile) {
    DWORD bytesRead;
    char buffer[BUFFER_SIZE];

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    while (ReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }
    printf("\n");
    return TRUE;
}

void CloseFile(HANDLE hFile) {
    if (hFile != NULL) {
        CloseHandle(hFile);
        hFile = NULL;
    }
}