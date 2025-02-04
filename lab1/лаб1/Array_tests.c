#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 100


void Parse_array(char s[MAX_LENGTH], char t, char before[MAX_LENGTH], char after[MAX_LENGTH]);

void test_Parse_array() {
    char before[MAX_LENGTH];
    char after[MAX_LENGTH];

    Parse_array("Hello, World!", ',', before, after);
    printf("\nTest1 - Before: %s, After: %s\n", before, after);

    Parse_array(NULL, ',', before, after);
    printf("\nTest2 - Before: %s, After: %s\n", before, after);

    Parse_array("qwertyuioplkjhgfdsazxcvbnm,.;lkjhgfdsaqwertyuiop;lkjhgfdsazxcvbnm,.lkjhgfdsawertyuioppoiuytrewsxdfghjkl;iuytrdsdftygvghnm", ',', before, after);
    printf("\nTest2 - Before: %s, After: %s\n", before, after);
}
