#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_NUMBERS 1024
#define MAX_PRIMES_SIZE 4096

char globalPrimes[MAX_PRIMES_SIZE];
int globalPrimesSize = 0;

pthread_mutex_t mutex;
__thread char tlsBuffer[1024];

void initialize() {
    pthread_mutex_init(&mutex, NULL);
    globalPrimes[0] = '\0';
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

void* ThreadFunc(void* lpParam) {
    int* params = (int*)lpParam;
    int start = params[0];
    int end = params[1];

    int current_buffer_size = 0;
    tlsBuffer[0] = '\0';

    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            current_buffer_size += snprintf(tlsBuffer + current_buffer_size, sizeof(tlsBuffer) - current_buffer_size, "%d ", i);
        }
    }

    pthread_mutex_lock(&mutex);

    printf("Founded primes: %s\n", tlsBuffer);

    if (globalPrimesSize + current_buffer_size < sizeof(globalPrimes)) {
        strcat(globalPrimes, tlsBuffer);
        globalPrimesSize += current_buffer_size;
    }
    else {
        printf("Global buffer overflow\n");
    }

    pthread_mutex_unlock(&mutex);
    free(params);

    return NULL;
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

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "usage: %s <process count> <lower_bound> <upper_bound>\n", argv[0]);
        return 1;
    }

    int numberOfProcess = atoi(argv[1]);
    int lower_bound = atoi(argv[2]);
    int upper_bound = atoi(argv[3]);
    pthread_t threads[10];

    static pthread_once_t onceControl = PTHREAD_ONCE_INIT;
    pthread_once(&onceControl, initialize);

    int range = upper_bound - lower_bound + 1;
    int step = range / numberOfProcess;

    for (int i = 0; i < numberOfProcess; i++) {
        int* params = (int*)malloc(2 * sizeof(int));
        params[0] = lower_bound + i * step;
        params[1] = (i == numberOfProcess - 1) ? upper_bound : params[0] + step - 1;

        if (pthread_create(&threads[i], NULL, ThreadFunc, params) != 0) {
            fprintf(stderr, "CreateThread error\n");
            free(params);
            return 1;
        }
    }

    for (int i = 0; i < numberOfProcess; i++) {
        pthread_join(threads[i], NULL);
    }

    sortGlobalPrimes();
    printf("\nPrimes array: %s\n", globalPrimes);

    pthread_mutex_destroy(&mutex);

    return 0;
}