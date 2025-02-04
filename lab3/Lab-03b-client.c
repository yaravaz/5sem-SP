#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <locale.h>
#include <semaphore.h>

#define BUFFER_SIZE 256
#define DEFAULT_FIFO_NAME "/tmp/my_fifo"
#define SEMAPHORE_NAME "/my_semaphore"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    strncpy(buffer, argv[1], sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    sem_t *sem = sem_open(SEMAPHORE_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return EXIT_FAILURE;
    }

    // Запись сообщения в FIFO
    int fd = open(DEFAULT_FIFO_NAME, O_WRONLY);
    write(fd, buffer, strlen(buffer) + 1);
    close(fd);

    // Чтение ответа из FIFO
    fd = open(DEFAULT_FIFO_NAME, O_RDONLY);
    read(fd, buffer, sizeof(buffer));
    close(fd);

    printf("Received from server: %s\n", buffer);

    sem_close(sem);
    return 0;
}