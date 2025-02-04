#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <wchar.h>

void ListDirectoryContents(const wchar_t* directory) {
    WIN32_FIND_DATAW findFileData;
    wchar_t searchPath[MAX_PATH];
    wcsncpy(searchPath, directory, MAX_PATH);
    wcscat(searchPath, L"\\*");

    HANDLE hFind = FindFirstFileW(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        wprintf(L"There is no catalog: %s\n", directory);
        exit(1);
    }

    do {
        const wchar_t* fileOrDirName = findFileData.cFileName;
        if (wcscmp(fileOrDirName, L".") != 0 && wcscmp(fileOrDirName, L"..") != 0) {
            wprintf(L"%s: %s\n",
                (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? L"[Folder]" : L"[File]",
                fileOrDirName);
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);
}

void WatchDirectory(const wchar_t* directory) {
    HANDLE hDir = CreateFileW(directory, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    if (hDir == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        wprintf(L"Error: the tracking folder could not be opened: %s (error code: %lu)\n", directory, error);
        return;
    }

    BYTE buffer[1024];
    DWORD bytesReturned;

    while (TRUE) {
        if (ReadDirectoryChangesW(hDir, buffer, sizeof(buffer), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
            FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
            &bytesReturned, NULL, NULL)) {

            FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)buffer;
            do {
                switch (fni->Action) {
                case FILE_ACTION_ADDED:
                    wprintf(L"Added: ");
                    break;
                case FILE_ACTION_REMOVED:
                    wprintf(L"Deleted: ");
                    break;
                case FILE_ACTION_MODIFIED:
                    wprintf(L"Edited: ");
                    break;
                case FILE_ACTION_RENAMED_OLD_NAME:
                    wprintf(L"Renamed (old): ");
                    break;
                case FILE_ACTION_RENAMED_NEW_NAME:
                    wprintf(L"Renamed (new): ");
                    break;
                }

                wprintf(L"%.*s\n", fni->FileNameLength / sizeof(WCHAR), fni->FileName);

                fni = fni->NextEntryOffset ? (FILE_NOTIFY_INFORMATION*)((BYTE*)fni + fni->NextEntryOffset) : NULL;
            } while (fni);
        }
    }

    CloseHandle(hDir);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s <catalog path>\n", argv[0]);
        return 1;
    }

    wchar_t directory[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, argv[1], -1, directory, MAX_PATH);

    ListDirectoryContents(directory);
    WatchDirectory(directory);

    return 0;
}