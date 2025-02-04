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

__declspec(dllexport) void createSampleFile(const char* filePath);
__declspec(dllexport) void OpenMapping(const char* filePath);
__declspec(dllexport) void AddRow(struct Student row, int pos);
__declspec(dllexport) void RemRow(int pos);
__declspec(dllexport) void PrintRow(int pos);
__declspec(dllexport) void PrintRows();
__declspec(dllexport) void CloseMapping();