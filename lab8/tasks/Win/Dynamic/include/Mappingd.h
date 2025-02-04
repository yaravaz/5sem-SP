#pragma once
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
HANDLE hFile;
struct Student* pStudents;
int studentCount;
struct Student students[MAX_STUDENTS];

void createSampleFile(const char* filePath);
void OpenMapping(const char* filePath);
void AddRow(struct Student row, int pos);
void RemRow(int pos);
void PrintRow(int pos);
void PrintRows();
void CloseMapping();

