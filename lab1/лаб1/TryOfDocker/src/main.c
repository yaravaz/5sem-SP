#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 100

void Parse_array(char s[MAX_LENGTH], char t, char before[MAX_LENGTH], char after[MAX_LENGTH]);
void Parse_pointer(const char* s, char t, char* before, char* after);
void test_Parse_array();
void test_Parse_pointer();

int main() {
    char str[MAX_LENGTH];
    char t;

    char before[MAX_LENGTH];
    char after[MAX_LENGTH];


    printf("Enter a string: ");
    scanf("%s", str);
    printf("Enter a character: ");
    scanf("\n%c", &t);

    printf("\nArray\n");

    Parse_array(str, t, before, after);
    printf("Before: %s\n", before);
    printf("After: %s\n", after);

    printf("\nPointer\n");

    Parse_pointer(str, t, before, after);
    printf("Before: %s\n", before);
    printf("After: %s\n", after);

    test_Parse_array();
    test_Parse_pointer();

    return 0;
}