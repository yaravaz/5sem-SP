#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int main() {

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    DWORD pageSize = sysInfo.dwPageSize;
    printf("Page size: %lu bytes\n", pageSize);

    size_t numPages = 256;
    size_t reserveSize = numPages * pageSize;
    LPVOID reservedMemory = VirtualAlloc(NULL, reserveSize, MEM_RESERVE, PAGE_NOACCESS);

    if (reservedMemory == NULL) {
        printf("error: %lu\n", GetLastError());
        return 1;
    }
    printf("address of virtual memory: %p\n", reservedMemory);
    printf("tap to continue...\n");
    getchar();

    LPVOID committedMemory = VirtualAlloc((LPVOID)((size_t)reservedMemory + (numPages / 2) * pageSize), numPages / 2 * pageSize, MEM_COMMIT, PAGE_READWRITE);

    if (committedMemory == NULL) {
        printf("error: %lu\n", GetLastError());
        VirtualFree(reservedMemory, 0, MEM_RELEASE);
        return 1;
    }
    printf("address of commited memory: %p\n", committedMemory);
    printf("tap to continue...\n");
    getchar();

    int* intArray = (int*)committedMemory;
    for (size_t i = 0; i < (numPages / 2) * (pageSize / sizeof(int)); i++) {
        intArray[i] = (int)i;
    }
    printf("commited memory is full of integers.\n");
    printf("tap to continue...\n");
    getchar();

    DWORD oldProtect;
    if (VirtualProtect(committedMemory, numPages / 2 * pageSize, PAGE_READONLY, &oldProtect) == 0) {
        printf("error: %lu\n", GetLastError());
        VirtualFree(reservedMemory, 0, MEM_RELEASE);
        return 1;
    }
    printf("readonly protection\n");
    printf("tap to continue...\n");
    getchar();

    if (VirtualFree(committedMemory, numPages / 2 * pageSize, MEM_DECOMMIT) == 0) {
        printf("error: %lu\n", GetLastError());
        VirtualFree(reservedMemory, 0, MEM_RELEASE);
        return 1;
    }
    printf("commited memory was freed\n");
    printf("tap to continue...\n");
    getchar();

    if (VirtualFree(reservedMemory, 0, MEM_RELEASE) == 0) {
        printf("error: %lu\n", GetLastError());
        return 1;
    }

    printf("reserved memory was freed\n");
    printf("tap to continue...\n");
    return 0;
}