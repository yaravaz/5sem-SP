#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>

void ListDirectoryContents(const char* directory) {
    struct dirent* entry;
    DIR* dp = opendir(directory);
    if (dp == NULL) {
        perror("Error opening directory");
        exit(1);
    }

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%s: %s\n", (entry->d_type == DT_DIR) ? "[Folder]" : "[File]", entry->d_name);
        }
    }

    closedir(dp);
}

void WatchDirectory(const char* directory) {
    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        return;
    }

    int wd = inotify_add_watch(fd, directory, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVE);
    if (wd == -1) {
        perror("inotify_add_watch");
        close(fd);
        return;
    }

    char buffer[1024];
    while (1) {
        int length = read(fd, buffer, sizeof(buffer));
        if (length < 0) {
            perror("read");
            break;
        }

        for (int i = 0; i < length; ) {
            struct inotify_event* event = (struct inotify_event*)&buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    printf("Added: %s\n", event->name);
                }
                if (event->mask & IN_DELETE) {
                    printf("Deleted: %s\n", event->name);
                }
                if (event->mask & IN_MODIFY) {
                    printf("Edited: %s\n", event->name);
                }
                if (event->mask & IN_MOVE) {
                    if (event->mask & IN_MOVED_FROM) {
                        printf("Renamed (old): %s\n", event->name);
                    }
                    if (event->mask & IN_MOVED_TO) {
                        printf("Renamed (new): %s\n", event->name);
                    }
                }
            }
            i += sizeof(struct inotify_event) + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s <directory path>\n", argv[0]);
        return 1;
    }

    ListDirectoryContents(argv[1]);
    WatchDirectory(argv[1]);

    return 0;
}