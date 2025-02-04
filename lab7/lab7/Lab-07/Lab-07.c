#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_STUDENTS 100

struct Student {
    char Name[64];       
    char Surname[128];   
    unsigned char Course;
    unsigned char Group; 
    char ID[8];          
};

HANDLE hFileMapping;
struct Student* pStudents;
int studentCount = MAX_STUDENTS;
struct Student students[MAX_STUDENTS];
HANDLE hFile;

void createSampleFile(const char* filePath) {

    for (int i = 0; i < 10; i++) {
        snprintf(students[i].Name, sizeof(students[i].Name), "Student%d", i + 1);
        snprintf(students[i].Surname, sizeof(students[i].Surname), "Surname%d", i + 1);
        students[i].Course = (unsigned char)(i % 5 + 1);
        students[i].Group = (unsigned char)(i % 3 + 1);
        snprintf(students[i].ID, sizeof(students[i].ID), "%04d", i + 1);
    }

    hFile = CreateFileA(filePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open file: %d\n", GetLastError());
        return;
    }

    DWORD bytesWritten;
    WriteFile(hFile, students, sizeof(struct Student) * MAX_STUDENTS, &bytesWritten, NULL);
    printf("Written %lu bytes to the file.\n", bytesWritten);

    CloseHandle(hFile);
}
void OpenMapping(LPSTR filePath, LPINT listSize) {

    hFile = CreateFileA(filePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("error CreateFileA: %d\n", GetLastError());
        return;
    }

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (!hFileMapping) {
        printf("Failed to create file mapping %d\n", GetLastError());
        return;
    }

    pStudents = (struct Student*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!pStudents) {
        printf("Failed to map view of file %d\n", GetLastError());
        return;
    }

}

void AddRow(HANDLE hFileMapping, struct Student row, INT pos) {
    if (hFileMapping == NULL) {
        printf("error: wrong handler.\n");
        return;
    }

    if (pos < 0) {
        pos = studentCount + pos;
    }

    if (pos < 0 || pos >= studentCount) {
        printf("wrong position: %d\n", pos);
        return;
    }

    if (strlen(pStudents[pos].Name) != 0) {
        printf("position %d is already busy.\n", pos);
        return;
    }

    pStudents[pos] = row;
}

void RemRow(HANDLE hFileMapping, INT pos) {
    if (hFileMapping == NULL) {
        printf("error: wrong handler.\n");
        return;
    }

    if (pos < 0) {
        pos = studentCount + pos;
    }

    if (pos < 0 || pos >= studentCount) {
        printf("wrong position: %d\n", pos);
        return;
    }

    if (strlen(pStudents[pos].Name) == 0) {
        printf("position %d is empty.\n", pos);
        return;
    }

    memset(&pStudents[pos], 0, sizeof(struct Student));
}

void PrintRow(HANDLE hFileMapping, INT pos) {
    if (hFileMapping == NULL) {
        printf("error: wrong handler.\n");
        return;
    }

    if (pos < 0) {
        pos = studentCount + pos;
    }

    if (pos < 0 || pos >= studentCount) {
        printf("wrong position: %d\n", pos);
        return;
    }

    if (strlen(pStudents[pos].Name) == 0) {
        printf("position %d is empty.\n", pos);
        return;
    }

    struct Student student = pStudents[pos];
    printf("Name: %s, LastName: %s, Course: %d, Group: %d, ID: %s\n",
        student.Name, student.Surname, student.Course, student.Group, student.ID);
}

void PrintRows(HANDLE hFileMapping) {
    if (hFileMapping == NULL) {
        printf("error: wrong handler\n");
        return;
    }

    for (int i = 0; i < studentCount; i++) {
        if (strlen(pStudents[i].Name) != 0) {
            PrintRow(hFileMapping, i);
        }
    }
}

void CloseMapping(HANDLE hFileMapping) {
    if (hFileMapping == NULL) {
        printf("error: wrong handler\n");
        return;
    }

    UnmapViewOfFile(pStudents);
    CloseHandle(hFileMapping);
    hFileMapping = NULL;
}


int main() {

    char filePath[MAX_PATH] = "D:\\лабы\\сп\\lab7\\students.bin";
    int pos;

    int listSize = 0;

    createSampleFile(filePath);
    OpenMapping(filePath, &listSize);

    int choice;
    struct Student student;

    do {
        printf("1. Add student\n");
        printf("2. Rem student\n");
        printf("3. Print row\n");
        printf("4. Print file\n");
        printf("5. close\n");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("Name: ");
            scanf("%s", student.Name);
            printf("Last name: ");
            scanf("%s", student.Surname);
            printf("Course: ");
            scanf("%hhu", &student.Course);
            printf("Group: ");
            scanf("%hhu", &student.Group);
            printf("ID: ");
            scanf("%s", student.ID);
            printf("Position: ");
            scanf("%d", &pos);
            AddRow(hFileMapping, student, pos);
            break;
        case 2:
            printf("position: ");
            scanf("%d", &pos);
            RemRow(hFileMapping, pos);
            break;
        case 3:
            printf("position: ");
            scanf("%d", &pos);
            PrintRow(hFileMapping, pos);
            break;
        case 4:
            PrintRows(hFileMapping);
            break;
        case 5:
            CloseMapping(hFileMapping);
            printf("exit...\n");
            break;
        default:
            printf("wrong option. try again...\n");
        }
    } while (choice != 5);

    return 0;
}