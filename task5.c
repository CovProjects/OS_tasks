#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>


const int BIN_WIDTH = 1024;
#define MAX_BINS 100

int bins[MAX_BINS] = {};


void categorize_file(const char *filepath) {
    struct stat file_stat;
    if (stat(filepath, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {     // makes sure stat runs and file is not a directory or a special file
        int bin_index = file_stat.st_size / BIN_WIDTH;
        if (bin_index < MAX_BINS) {
            bins[bin_index]++;
        }
    }
}

void traverse_directory(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("Failed to open dir \n");
        return;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "%s/%s", dirpath, ent->d_name);   // constructs the file path by combining the directory and file name

        struct stat path_stat;
        if (stat(file_path, &path_stat) == 0) {
            if (S_ISDIR(path_stat.st_mode)) {
                traverse_directory(file_path);     // get files within subdirectories
            } else if (S_ISREG(path_stat.st_mode)) {
                categorize_file(file_path);
            }
        }
    }
}

void print_histogram() {
    printf("Histogram: \n");
    for (int i = 0; i < MAX_BINS; i++) {
        if (bins[i] > 0) {
            printf("%d - %d bytes: %d files\n", i * BIN_WIDTH, (i + 1) * BIN_WIDTH - 1, bins[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    traverse_directory(argv[1]);
    print_histogram();
    return 0;
}
