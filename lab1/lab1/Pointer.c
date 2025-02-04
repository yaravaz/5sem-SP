#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#define MAX_LENGTH 100

void Parse_pointer(const char* s, char t, char* before, char* after) {
    before[0] = '\0';
    after[0] = '\0';

    if (!s) {
        *before = NULL;
        *after = NULL;
        printf("\nString is undefinded\n");
        return;
    }
    else if (strlen(s) > MAX_LENGTH) {
        printf("\nData can be cropped: too many characters\n");
    }

    const char* pos = s;

    while (*pos && *pos != t) {
        pos++;
    }

    if (*pos == '\0') {
        int i = 0;
        while (s[i] != '\0' && i < MAX_LENGTH - 1) {
            before[i] = s[i];
            i++;
        }
        before[i] = '\0';
        return;
    }

    size_t len_before = pos - s;
    for (size_t i = 0; i < len_before && i < MAX_LENGTH - 1; i++) {
        before[i] = s[i];
    }
    before[len_before] = '\0';

    if (*(pos + 1) != '\0') {
        const char* start_after = pos + 1;
        size_t j = 0;
        while (start_after[j] != '\0' && j < MAX_LENGTH - 1) {
            after[j] = start_after[j];
            j++;
        }
        after[j] = '\0'; 
    }
    else {
        after[0] = '\0';
    }
}