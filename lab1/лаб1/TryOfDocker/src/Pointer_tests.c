#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 100

void Parse_pointer(const char* s, char t, char* before, char* after);

void test_Parse_pointer() {
    char before[MAX_LENGTH];
    char after[MAX_LENGTH];

    Parse_pointer("Hello, World!", ',', before, after);
    printf("\nTest1 - Before: %s, After: %s\n", before, after);

    Parse_pointer(NULL, ',', before, after);
    printf("\nTest2 - Before: %s, After: %s\n", before, after);

    Parse_pointer("qwertyuioplkjhgfdsazxcvbnm,.;lkjhgfdsaqwertyuiop;lkjhgfdsazxcvbnm,.lkjhgfdsawertyuioppoiuytrewsxdfghjkl;iuytrdsdftygvghnm", ',', before, after);
    printf("\nTest2 - Before: %s, After: %s\n", before, after);
}