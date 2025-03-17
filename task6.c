#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

void print_directory(char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("Failed to open dir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        printf("%s\n", entry->d_name);
    }
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        printf("%s\n", argv[i]);
        print_directory(argv[i]);
        printf("\n");
    }
    return 0;
}
