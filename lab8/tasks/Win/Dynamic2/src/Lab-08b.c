#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "Mappingd-2.h"

typedef void (*CreateSampleFileType)(const char*);
typedef void (*OpenMappingType)(const char*);
typedef void (*AddRowType)(struct Student, int);
typedef void (*RemRowType)(int);
typedef void (*PrintRowType)(int);
typedef void (*PrintRowsType)();
typedef void (*CloseMappingType)();

int main() {
    HMODULE hLib = LoadLibraryA("Mappingd-2.dll");
    if (!hLib) {
        printf("Failed to load dll: %d\n", (int)GetLastError());
        return 1;
    }

    CreateSampleFileType createSampleFile = (CreateSampleFileType)GetProcAddress(hLib, "createSampleFile");
    OpenMappingType openMapping = (OpenMappingType)GetProcAddress(hLib, "OpenMapping");
    AddRowType addRow = (AddRowType)GetProcAddress(hLib, "AddRow");
    RemRowType remRow = (RemRowType)GetProcAddress(hLib, "RemRow");
    PrintRowType printRow = (PrintRowType)GetProcAddress(hLib, "PrintRow");
    PrintRowsType printRows = (PrintRowsType)GetProcAddress(hLib, "PrintRows");
    CloseMappingType closeMapping = (CloseMappingType)GetProcAddress(hLib, "CloseMapping");

    if (!createSampleFile || !openMapping || !addRow || !remRow || !printRow || !printRows || !closeMapping) {
        printf("Failed to get function address from Mappingd.dll: %d\n", (int)GetLastError());
        FreeLibrary(hLib);
        return 1;
    }

    char filePath[260] = "students.bin";
    int pos;
    struct Student student;

    createSampleFile(filePath);
    openMapping(filePath);

    int choice;
    do {
        printf("1. Add student\n");
        printf("2. Rem student\n");
        printf("3. Print row\n");
        printf("4. Print file\n");
        printf("5. Close\n");
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
            addRow(student, pos);
            break;
        case 2:
            printf("Position: ");
            scanf("%d", &pos);
            remRow(pos);
            break;
        case 3:
            printf("Position: ");
            scanf("%d", &pos);
            printRow(pos);
            break;
        case 4:
            printRows();
            break;
        case 5:
            closeMapping();
            printf("Exiting...\n");
            break;
        default:
            printf("Wrong option. Try again...\n");
        }
    } while (choice != 5);

    FreeLibrary(hLib);

    return 0;
}