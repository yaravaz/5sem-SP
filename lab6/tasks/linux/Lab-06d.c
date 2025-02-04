#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

void PrintFileType(mode_t mode) {
    if (S_ISREG(mode)) {
        fputs("Regular file.\n", stdout);
    }
    else if (S_ISDIR(mode)) {
        fputs("Directory.\n", stdout);
    }
    else if (S_ISCHR(mode)) {
        fputs("Character device.\n", stdout);
    }
    else if (S_ISBLK(mode)) {
        fputs("Block device.\n", stdout);
    }
    else if (S_ISFIFO(mode)) {
        fputs("FIFO (named pipe).\n", stdout);
    }
    else if (S_ISLNK(mode)) {
        fputs("Symbolic link.\n", stdout);
    }
    else if (S_ISSOCK(mode)) {
        fputs("Socket.\n", stdout);
    }
    else {
        fputs("Unknown type file.\n", stdout);
    }
}

void PrintInfo(const char* fileName) {
    struct stat fileInfo;

    if (stat(fileName, &fileInfo) != 0) {
        perror("Error getting file information");
        return;
    }

    printf("File Name: %s\n", fileName);
    printf("File size: %lld bytes (%.2f KiB, %.2f MiB)\n",
        (long long)fileInfo.st_size,
        fileInfo.st_size / 1024.0,
        fileInfo.st_size / (1024.0 * 1024.0));

    PrintFileType(fileInfo.st_mode);

    char creationTime[20], lastAccessTime[20], lastWriteTime[20];
    strftime(creationTime, sizeof(creationTime), "%d.%m.%Y %H:%M:%S", localtime(&fileInfo.st_ctime));
    strftime(lastAccessTime, sizeof(lastAccessTime), "%d.%m.%Y %H:%M:%S", localtime(&fileInfo.st_atime));
    strftime(lastWriteTime, sizeof(lastWriteTime), "%d.%m.%Y %H:%M:%S", localtime(&fileInfo.st_mtime));

    printf("Creation Time: %s\n", creationTime);
    printf("Last Access Time: %s\n", lastAccessTime);
    printf("Last Write Time: %s\n", lastWriteTime);
}

void PrintRows(const char* fileName) {
    int fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    char buffer[512];
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }

    close(fd);
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