#include <windows.h>
#include <stdio.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

void PrintFileType(DWORD dwFileType) {
    switch (dwFileType) {
    case FILE_TYPE_UNKNOWN:
        fputs("Unknown type file.\n", stdout);
        break;
    case FILE_TYPE_DISK:
        fputs("Disk type file.\n", stdout);
        break;
    case FILE_TYPE_CHAR:
        fputs("Char type file.\n", stdout);
        break;
    case FILE_TYPE_PIPE:
        fputs("Pipe type file.\n", stdout);
        break;
    default:
        break;
    }
}

void PrintInfo(const char* fileName) {
    if (!PathFileExistsA(fileName)) {
        printf("File does not exist: %s\n", fileName);
        return;
    }

    HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Error opening file: %s\n", fileName);
        return;
    }

    BY_HANDLE_FILE_INFORMATION fileInfo;
    if (!GetFileInformationByHandle(hFile, &fileInfo)) {
        printf("Error getting file information: %s\n", fileName);
        CloseHandle(hFile);
        return;
    }

    const char* fileNameOnly = PathFindFileNameA(fileName);
    printf("File Name: %s\n", fileNameOnly);

    LARGE_INTEGER fileSize;
    fileSize.HighPart = fileInfo.nFileSizeHigh;
    fileSize.LowPart = fileInfo.nFileSizeLow;

    printf("File size: %lld bytes (%.2f KiB, %.2f MiB)\n",
        fileSize.QuadPart,
        fileSize.QuadPart / 1024.0,
        fileSize.QuadPart / (1024.0 * 1024.0));

    DWORD dwFileType = GetFileType(hFile);
    PrintFileType(dwFileType);

    SYSTEMTIME creationTime, lastAccessTime, lastWriteTime;
    FileTimeToSystemTime(&fileInfo.ftCreationTime, &creationTime);
    FileTimeToSystemTime(&fileInfo.ftLastAccessTime, &lastAccessTime);
    FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &lastWriteTime);

    printf("Creation Time: %02d.%02d.%d %02d:%02d:%02d\n",
        creationTime.wDay, creationTime.wMonth, creationTime.wYear,
        creationTime.wHour, creationTime.wMinute, creationTime.wSecond);
    printf("Last Access Time: %02d.%02d.%d %02d:%02d:%02d\n",
        lastAccessTime.wDay, lastAccessTime.wMonth, lastAccessTime.wYear,
        lastAccessTime.wHour, lastAccessTime.wMinute, lastAccessTime.wSecond);
    printf("Last Write Time: %02d.%02d.%d %02d:%02d:%02d\n",
        lastWriteTime.wDay, lastWriteTime.wMonth, lastWriteTime.wYear,
        lastWriteTime.wHour, lastWriteTime.wMinute, lastWriteTime.wSecond);

    wchar_t wideFileName[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, fileName, -1, wideFileName, MAX_PATH);

    DWORD dwBinaryType;
    if (!GetBinaryType(wideFileName, &dwBinaryType)) {
        fprintf(stderr, "The file may not be executable.\n");
        CloseHandle(hFile);
        return;
    }
    else {
        switch (dwBinaryType) {
        case SCS_32BIT_BINARY:
            fputs("A 32-bit Windows-based application.\n", stdout);
            break;
        case SCS_64BIT_BINARY:
            fputs("A 64-bit Windows-based application.\n", stdout);
            break;
        case SCS_DOS_BINARY:
            fputs("An MS-DOS – based application.\n", stdout);
            break;
        case SCS_OS216_BINARY:
            fputs("A 16-bit OS/2-based application.\n", stdout);
            break;
        case SCS_PIF_BINARY:
            fputs("A PIF file that executes an MS-DOS – based application.\n", stdout);
            break;
        case SCS_POSIX_BINARY:
            fputs("A POSIX – based application.\n", stdout);
            break;
        case SCS_WOW_BINARY:
            fputs("A 16-bit Windows-based application.\n", stdout);
            break;
        default:
            fputs("Mystery application.\n", stdout);
            break;
        }
    }

    CloseHandle(hFile);
}

void PrintRows(const char* fileName) {
    HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Error opening file: %s\n", fileName);
        return;
    }
    DWORD bytesRead;
    char buffer[512];

    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    while (ReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }

    CloseHandle(hFile);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file path>\n", argv[0]);
        return 1;
    }

    PrintInfo(argv[1]);
    PrintRows(argv[1]);

    return 0;
}
