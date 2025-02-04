#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_STUDENTS 100

struct Student {
    char Name[64];
    char Surname[128];
    unsigned char Course;
    unsigned char Group;
    char ID[8];
};

struct Student* pStudents;
int studentCount = MAX_STUDENTS;

void createSampleFile(const char* filePath) {
    struct Student students[MAX_STUDENTS];
    memset(students, 0, sizeof(students)); // Инициализация массива

    for (int i = 0; i < 10; i++) {
        snprintf(students[i].Name, sizeof(students[i].Name), "Student%d", i + 1);
        snprintf(students[i].Surname, sizeof(students[i].Surname), "Surname%d", i + 1);
        students[i].Course = (unsigned char)(i % 5 + 1);
        students[i].Group = (unsigned char)(i % 3 + 1);
        snprintf(students[i].ID, sizeof(students[i].ID), "%04d", i + 1);
    }

    int hFile = open(filePath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (hFile < 0) {
        perror("Failed to create file");
        return;
    }

    write(hFile, students, sizeof(struct Student) * MAX_STUDENTS);
    close(hFile);
}

void OpenMapping(const char* filePath) {
    int hFile = open(filePath, O_RDWR);
    if (hFile < 0) {
        perror("Error opening file");
        return;
    }

    struct stat sb;
    if (fstat(hFile, &sb) == -1) {
        perror("fstat failed");
        close(hFile);
        return;
    }

    pStudents = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, hFile, 0);
    if (pStudents == MAP_FAILED) {
        perror("Failed to map view of file");
        close(hFile);
        return;
    }

    close(hFile);
}

void AddRow(struct Student row, int pos) {
    if (pos < 0) {
        pos = studentCount + pos;
    }

    if (pos < 0 || pos >= studentCount) {
        printf("Wrong position: %d\n", pos);
        return;
    }

    if (strlen(pStudents[pos].Name) != 0) {
        printf("Position %d is already busy.\n", pos);
        return;
    }

    pStudents[pos] = row;
}

void RemRow(int pos) {
    if (pos < 0) {
        pos = studentCount + pos;
    }

    if (pos < 0 || pos >= studentCount) {
        printf("Wrong position: %d\n", pos);
        return;
    }

    if (strlen(pStudents[pos].Name) == 0) {
        printf("Position %d is empty.\n", pos);
        return;
    }

    memset(&pStudents[pos], 0, sizeof(struct Student));
}

void PrintRow(int pos) {
    if (pos < 0) {
        pos = studentCount + pos;
    }

    if (pos < 0 || pos >= studentCount) {
        printf("Wrong position: %d\n", pos);
        return;
    }

    if (strlen(pStudents[pos].Name) == 0) {
        printf("Position %d is empty.\n", pos);
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
    if (munmap(pStudents, MAX_STUDENTS * sizeof(struct Student)) == -1) {
        perror("Failed to unmap view of file");
    }
}

int main() {
    const char* filePath = "students.bin";
    int pos;

    createSampleFile(filePath);
    OpenMapping(filePath);

    int choice;
    struct Student student;

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
