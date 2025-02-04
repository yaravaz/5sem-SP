#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "library/include/Mappings.h"

int main() {
    char filePath[260] = "students.bin";
    int pos;
    struct Student student;

    createSampleFile(filePath);
    OpenMapping(filePath);

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
            AddRow(student, pos);
            break;
        case 2:
            printf("Position: ");
            scanf("%d", &pos);
            RemRow(pos);
            break;
        case 3:
            printf("Position: ");
            scanf("%d", &pos);
            PrintRow(pos);
            break;
        case 4:
            PrintRows();
            break;
        case 5:
            CloseMapping();
            printf("Exiting...\n");
            break;
        default:
            printf("Wrong option. Try again...\n");
        }
    } while (choice != 5);

    return 0;
}