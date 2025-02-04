#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <wtypes.h>

int main(int argc, char* argv[]) {
    int iterationCount;

    /*if (argc != 2) {
        fprintf(stderr, "The number of iterations is not specified.\n");
        const char* envirVar = getenv("ITER_NUM");
        if (envirVar != NULL) {
            iterationCount = atoi(envirVar);
            printf("Taking data from the environment variable.\n");
        }
        else {
            fprintf(stderr, "No data available\n");
            ExitProcess(1);
        }
    }
    else {
        iterationCount = atoi(argv[1]);
    }*/

    if (argc != 2) {
        fprintf(stderr, "The number of iterations is not specified.\n");
        ExitProcess(1);
    }

    iterationCount = atoi(argv[1]);

    if (iterationCount <= 0) {
        fprintf(stderr, "number of iterations < 0.\n");
        ExitProcess(1);
    }

    DWORD pid = GetCurrentProcessId();
    for (int i = 0; i < iterationCount; i++) {
        printf("%d PID: %lu\n", i + 1, pid);
        Sleep(500);
    }
    return 0;
}