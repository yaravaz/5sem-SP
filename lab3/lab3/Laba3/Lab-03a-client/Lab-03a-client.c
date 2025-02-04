#include <stdio.h>
#include <windows.h>
#include <locale.h>

int primes_buffer[1024];
int primes_buffer_size = 0;

BOOL isPrime(int num) {
    if (num <= 1)
        return FALSE;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0)
            return FALSE;
    }
    return TRUE;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    if (argc < 3) {
        fprintf(stderr, "usage: %s <lower bound> <upper bound> [mutex name]\n", argv[0]);
        return 1;
    }

    int lowerBound = atoi(argv[1]);
    int upperBound = atoi(argv[2]);
    const char* mutex_name = (argc > 3) ? argv[3] : "Default";

    wchar_t wideMutexName[256];
    MultiByteToWideChar(CP_UTF8, 0, mutex_name, -1, wideMutexName, sizeof(wideMutexName) / sizeof(wideMutexName[0]));

    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, wideMutexName);
    if (hMutex == NULL) {
        fprintf(stderr, "OpenMutex error: %ld\n", GetLastError());
        return 1;
    }

    for (int i = lowerBound; i <= upperBound; ++i) {
        if (isPrime(i)) {
            primes_buffer[primes_buffer_size] = i;
            primes_buffer_size++;
        }
    }

    WaitForSingleObject(hMutex, INFINITE);
    for (size_t i = 0; i < primes_buffer_size; i++) {
        printf("%d ", primes_buffer[i]);
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    return 0;
}