#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 100

void Parse_array(char s[MAX_LENGTH], char t, char before[MAX_LENGTH], char after[MAX_LENGTH]) {

    int i;

    if (!s) {
        *before = NULL;
        *after = NULL;
        printf("\nString is undefinded\n");
        return;
    }
    else if (strlen(s) > MAX_LENGTH) {
        printf("\nData can be cropped: too many characters\n");
    }

    for (i = 0; i < MAX_LENGTH && s[i] != '\0' && s[i] != t; i++);

    if (s[i] == '\0') {
        for (int j = 0; j < i && j < MAX_LENGTH - 1; j++) {
            before[j] = s[j];
        }
        before[i] = '\0';
        after[0] = '\0';
        return;
    }

    for (int j = 0; j < i && j < MAX_LENGTH - 1; j++) {
        before[j] = s[j];
    }
    before[i] = '\0';

    int j = 0;
    for (i = i + 1; i < MAX_LENGTH && s[i] != '\0'; i++, j++) {
        if (j < MAX_LENGTH - 1) {
            after[j] = s[i];
        }
    }
    after[j] = '\0';
}