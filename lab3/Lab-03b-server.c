#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <locale.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <ctype.h>

#define BUFFER_SIZE 256
#define DEFAULT_FIFO_NAME "/tmp/my_fifo"
#define SEMAPHORE_NAME "/my_semaphore"

void to_upper_case(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    setlocale(LC_ALL, "");

    char buffer[BUFFER_SIZE];
    mkfifo(DEFAULT_FIFO_NAME, 0666);  // Создание FIFO

    sem_t *sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for client connection...\n");

    while (1) {
        int fd = open(DEFAULT_FIFO_NAME, O_RDONLY);  // Открытие FIFO для чтения
        read(fd, buffer, sizeof(buffer));
        close(fd);

        printf("Received: %s\n", buffer);
        to_upper_case(buffer);
        printf("Sending back: %s\n", buffer);

        sem_wait(sem);  // Захват семафора

        fd = open(DEFAULT_FIFO_NAME, O_WRONLY);  // Открытие FIFO для записи
        write(fd, buffer, strlen(buffer) + 1);
        close(fd);

        sem_post(sem);  // Освобождение семафора
        printf("\n");
    }

    sem_close(sem);
    sem_unlink(SEMAPHORE_NAME);
    return 0;
}