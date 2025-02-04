#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define INITIAL_HEAP_SIZE (1 * 1024 * 1024)
#define MAX_HEAP_SIZE (8 * 1024 * 1024)
#define BLOCK_SIZE (512 * 1024) // 1024 * 1024
#define NUM_BLOCKS 10 //5

void HeapInfo(HANDLE heap) {

    PROCESS_HEAP_ENTRY entry;
    entry.lpData = NULL;

    while (HeapWalk(heap, &entry)) {
        if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) {
            printf("Address: %p, Size: %zu, Type: Busy\n", entry.lpData, entry.cbData);
        }
        else {
            printf("Address: %p, Size: %zu, Type: Free\n", entry.lpData, entry.cbData);
        }
    }
    printf("\n");
}

int main() {

    HANDLE heap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, INITIAL_HEAP_SIZE, MAX_HEAP_SIZE);
    if (heap == NULL) {
        printf("error: %lu\n", GetLastError());
        return 1;
    }
    printf("heap was created\n");
    HeapInfo(heap);
    system("pause & cls");

    void* blocks[NUM_BLOCKS];
    for (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i] = HeapAlloc(heap, HEAP_GENERATE_EXCEPTIONS, BLOCK_SIZE);
        if (blocks[i] == NULL) {
            printf("error: %lu\n", GetLastError());
            HeapDestroy(heap);
            return 1;
        }
        int* intArray = (int*)blocks[i];
        size_t numIntegers = BLOCK_SIZE / sizeof(int);
        for (size_t j = 0; j < numIntegers; j++) {
            intArray[j] = (int)(j + i * numIntegers);
        }
        printf("block %d created and filled.\n", i);
        HeapInfo(heap);
        system("pause & cls");
    }

    for (int i = 0; i < NUM_BLOCKS; i++) {
        HeapFree(heap, 0, blocks[i]);
        printf("block %d was freed.\n", i);
        HeapInfo(heap);
        system("pause & cls");
    }

    if (HeapDestroy(heap) == 0) {
        printf("error: %lu\n", GetLastError());
        return 1;
    }
    printf("heap was destroyed.\n");

    return 0;
}