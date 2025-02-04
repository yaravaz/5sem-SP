#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BLOCK_SIZE (512 * 1024)
#define NUM_BLOCKS 10

int main() {

    void* blocks[NUM_BLOCKS];

    for (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i] = malloc(BLOCK_SIZE);
        if (blocks[i] == NULL) {
            perror("error malloc");
            for (int j = 0; j < i; j++) {
                free(blocks[j]);
            }
            return 1;
        }

        int* intArray = (int*)blocks[i];
        size_t numIntegers = BLOCK_SIZE / sizeof(int);
        for (size_t j = 0; j < numIntegers; j++) {
            intArray[j] = (int)(j + i * numIntegers);
        }
        printf("block %d allocated and filled\n", i);
        printf("tap to continue...\n");
        getchar();
    }

    for (int i = 0; i < NUM_BLOCKS; i++) {
        free(blocks[i]);
        printf("block %d freed\n", i);
        printf("tap to continue...\n");
        getchar();
    }

    printf("all blocks freed\n");
    return 0;
}