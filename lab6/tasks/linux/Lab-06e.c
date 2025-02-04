#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int hFile = -1;

int OpenFiles(const char* fileName);
int AddRow(int hFile, const char* str, int pos);
int RemRow(int hFile, int pos);
int PrintRow(int hFile, int pos);
int PrintRows(int hFile);
void CloseFile(int hFile);

int main(int argc, char* argv[]) {
    char row[BUFFER_SIZE];
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
            fgets(row, BUFFER_SIZE, stdin);
            size_t len = strlen(row);
            if (len > 0 && row[len - 1] == '\n') {
                row[len - 1] = '\0';
            }
            if (strlen(row) == 0) {
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
    hFile = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (hFile == -1) {
        perror("Error opening file");
        return errno;
    }
    printf("File '%s' successfully opened.\n", fileName);
    return 0;
}

int AddRow(int hFile, const char* str, int pos) {
    struct stat fileInfo;
    if (fstat(hFile, &fileInfo) == -1) {
        perror("Error getting file size");
        return -1;
    }

    int strLength = strlen(str);
    char* buf = (char*)malloc(fileInfo.st_size + 1);
    if (buf == NULL) {
        perror("Memory allocation error");
        return -1;
    }
    lseek(hFile, 0, SEEK_SET);
    read(hFile, buf, fileInfo.st_size);
    buf[fileInfo.st_size] = '\0';

    char* result = (char*)malloc(fileInfo.st_size + strLength + 3);
    if (result == NULL) {
        free(buf);
        perror("Memory allocation error");
        return -1;
    }

    result[0] = '\0';
    size_t position = 0;
    int rowCount = 1;

    while (position < fileInfo.st_size) {
        if (rowCount == pos) {
            strcat(result, str);
            strcat(result, "\n");
        }

        while (position < fileInfo.st_size && buf[position] != '\n') {
            strncat(result, &buf[position], 1);
            position++;
        }
        if (position < fileInfo.st_size) {
            strncat(result, "\n", 1);
            position++;
        }
        rowCount++;
    }

    if (pos == -1) {
        strcat(result, str);
        strcat(result, "\n");
    }

    if (pos == 0) {
        char* temp = (char*)malloc(strlen(result) + strlen(str) + 2);
        sprintf(temp, "%s\n%s", str, result);
        free(result);
        result = temp;
    }

    lseek(hFile, 0, SEEK_SET);
    write(hFile, result, strlen(result));
    ftruncate(hFile, strlen(result));

    free(buf);
    free(result);
    return 1;
}

int CountRows(int hFile, char* ptr) {
    int rowCount = 0;
    while (*ptr) {
        if (*ptr == '\n') {
            rowCount++;
        }
        ptr++;
    }
    lseek(hFile, 0, SEEK_SET);
    return rowCount;
}

int RemRow(int hFile, int pos) {
    struct stat fileInfo;
    if (fstat(hFile, &fileInfo) == -1) {
        perror("Error getting file size");
        return -1;
    }

    lseek(hFile, 0, SEEK_SET);
    char* buf = (char*)malloc(fileInfo.st_size + 1);
    if (buf == NULL) {
        perror("Memory allocation error");
        return -1;
    }
    read(hFile, buf, fileInfo.st_size);
    buf[fileInfo.st_size] = '\0';

    char* result = (char*)malloc(fileInfo.st_size + 1);
    result[0] = '\0';
    int rowCount = 1;
    char* ptr = buf;
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

    lseek(hFile, 0, SEEK_SET);
    write(hFile, result, strlen(result));
    ftruncate(hFile, strlen(result));

    free(buf);
    free(result);
    return 1;
}

int PrintRow(int hFile, int pos) {
    struct stat fileInfo;
    if (fstat(hFile, &fileInfo) == -1) {
        perror("Error getting file size");
        return -1;
    }

    char* buf = (char*)malloc(fileInfo.st_size + 1);
    if (buf == NULL) {
        perror("Memory allocation error");
        return -1;
    }
    lseek(hFile, 0, SEEK_SET);
    read(hFile, buf, fileInfo.st_size);
    buf[fileInfo.st_size] = '\0';

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
    return 1;
}

int PrintRows(int hFile) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    lseek(hFile, 0, SEEK_SET);
    while ((bytesRead = read(hFile, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }
    printf("\n");
    return 1;
}

void CloseFile(int hFile) {
    if (hFile != -1) {
        close(hFile);
        hFile = -1;
    }
}