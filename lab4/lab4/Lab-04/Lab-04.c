#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#pragma warning(disable : 4996)

CRITICAL_SECTION cs;
DWORD dwTlsIndex;

#define MAX_NUMBERS 1024
#define MAX_PRIMES_SIZE 4096

char globalPrimes[MAX_PRIMES_SIZE];
int globalPrimesSize = 0;

VOID ErrorExit(LPCSTR message)
{
    fprintf(stderr, "%s\n", message);
    ExitProcess(0);
}

int is_prime(int num) {
    if (num < 2) return 0;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}

DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
    int* params = (int*)lpParam;
    int start = params[0];
    int end = params[1];

    int current_buffer_size = 0;
    int buffer_capacity = 1024;

    char* buffer = (char*)LocalAlloc(LPTR, buffer_capacity * sizeof(char));
    if (buffer == NULL) {
        ErrorExit("LocalAlloc failed");
    }

    if (!TlsSetValue(dwTlsIndex, buffer)) {
        ErrorExit("TlsSetValue error");
    }

    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            current_buffer_size += snprintf(buffer + current_buffer_size, buffer_capacity - current_buffer_size, "%d ", i);
        }
    }

    EnterCriticalSection(&cs);

    char* tlsBuffer = (char*)TlsGetValue(dwTlsIndex);
    if (tlsBuffer == NULL) {
        ErrorExit("TlsGetValue error");
    }

    _cprintf("Founded primes: %s\n", tlsBuffer);

    if (globalPrimesSize + current_buffer_size < sizeof(globalPrimes)) {
        strcat(globalPrimes, tlsBuffer);
        globalPrimesSize += current_buffer_size;
    }
    else {
        _cprintf("Global buffer overflow\n");
    }

    LeaveCriticalSection(&cs);
    LocalFree(buffer);
    free(params);

    return 0;
}

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void sortGlobalPrimes() {
    int numbers[MAX_NUMBERS];
    int numCount = 0;

    char* token = strtok(globalPrimes, " ");
    while (token != NULL && numCount < MAX_NUMBERS) {
        numbers[numCount++] = atoi(token);
        token = strtok(NULL, " ");
    }
    qsort(numbers, numCount, sizeof(int), compare);

    globalPrimes[0] = '\0';
    for (int i = 0; i < numCount; i++) {
        char temp[16];
        snprintf(temp, sizeof(temp), "%d ", numbers[i]);
        strcat(globalPrimes, temp);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        fprintf(stderr, "usage: %s <process count> <lower_bound> <upper_bound>\n", argv[0]);
        return 1;
    }

    int numberOfProcess = atoi(argv[1]);
    int lower_bound = atoi(argv[2]);
    int upper_bound = atoi(argv[3]);
    DWORD IDThread;
    HANDLE hThread[10];
    int i;

    InitializeCriticalSection(&cs);

    int range = upper_bound - lower_bound + 1;
    int step = range / numberOfProcess;

    if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)
        ErrorExit("TlsAlloc failed");

    for (i = 0; i < numberOfProcess; i++)
    {
        int* params = (int*)malloc(2 * sizeof(int));
        params[0] = lower_bound + i * step;
        params[1] = (i == numberOfProcess - 1) ? upper_bound : params[0] + step - 1;

        hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, params, 0, &IDThread);

        if (hThread[i] == NULL)
            ErrorExit("CreateThread error\n");
    }

    WaitForMultipleObjects(numberOfProcess, hThread, TRUE, INFINITE);
    TlsFree(dwTlsIndex);
    DeleteCriticalSection(&cs);
    sortGlobalPrimes();
    _cprintf("\nPrimes array: %s\n", globalPrimes);

    return 0;
}