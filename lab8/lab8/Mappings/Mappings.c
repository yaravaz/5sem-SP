#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "Mappings.h"

HANDLE hFileMapping, hFile;
struct Student* pStudents;
int studentCount = MAX_STUDENTS;
struct Student students[MAX_STUDENTS];

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
void OpenMapping(const char* filePath) {

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

void AddRow(struct Student row, int pos) {

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

void RemRow(int pos) {

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

void PrintRow(int pos) {

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

void PrintRows() {

    for (int i = 0; i < studentCount; i++) {
        if (strlen(pStudents[i].Name) != 0) {
            PrintRow(i);
        }
    }
}

void CloseMapping() {

    UnmapViewOfFile(pStudents);
    CloseHandle(hFileMapping);
    hFileMapping = NULL;
}
