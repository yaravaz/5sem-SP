#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define NUM_PAGES 256
#define PAGE_SIZE sysconf(_SC_PAGESIZE) 


int main() {
    size_t page_size = PAGE_SIZE;
    size_t total_size = NUM_PAGES * page_size;

    void* reserved_memory = mmap(NULL, total_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (reserved_memory == MAP_FAILED) {
        perror("error mmap");
        return 1;
    }
    printf("memory was reserved in %p\n", reserved_memory);
    printf("tap to continue...\n");
    getchar();

    if (mprotect((char*)reserved_memory + (NUM_PAGES / 2) * page_size, total_size / 2, PROT_READ | PROT_WRITE) == -1) {
        perror("error mprotect");
        munmap(reserved_memory, total_size);
        return 1;
    }
    printf("physical memory was allocated\n");
    printf("tap to continue...\n");
    getchar();

    int* int_array = (int*)((char*)reserved_memory + (NUM_PAGES / 2) * page_size);
    for (size_t i = 0; i < (NUM_PAGES / 2) * (page_size / sizeof(int)); i++) {
        int_array[i] = (int)i; 
    }
    printf("memoery filled with integers\n");
    printf("tap to continue...\n");
    getchar();

    if (mprotect((char*)reserved_memory + (NUM_PAGES / 2) * page_size, total_size / 2, PROT_READ) == -1) {
        perror("error mprotect(prot_read)");
        munmap(reserved_memory, total_size);
        return 1;
    }
    printf("protection was changed to read-only\n");
    printf("tap to continue...\n");
    getchar();

    if (munmap(int_array, total_size / 2) == -1) {
        perror("error munmap(physical)");
        munmap(reserved_memory, total_size);
        return 1;
    }
    printf("physical memory was freed\n");
    printf("tap to continue...\n");
    getchar();

    if (munmap(reserved_memory, total_size) == -1) {
        perror("error munmap(reserved)");
        return 1;
    }
    printf("reserved memory was released\n");

    return 0;
}