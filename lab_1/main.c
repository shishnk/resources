#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Console colors
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define YELLOW_BOLD "\033[1;33m"
#define NC "\033[0m"

// Group: ПМ-92
// Task version: 8
// Students: Begichev, Shishkin

// From Linux man page
int cmpstringp(const void *p1, const void *p2) {
    return strcmp(*(const char **)p1, *(const char **)p2);
}

int show_dir(const char *dirname) {
    DIR *dir;
    struct stat stbuf;
    struct dirent *item;
    unsigned int iters = 0;

    unsigned int max_file_count = 1;
    unsigned int max_dir_count = 1;
    char **file_names = (char **)malloc(max_file_count * sizeof(char *));
    char **dir_names = (char **)malloc(max_dir_count * sizeof(char *));

    // dir name length
    size_t dirname_len = strlen(dirname);

    // curent path
    char path[PATH_MAX];
    strcpy(path, dirname);
    strcat(path, "/");

    // Count files and dirs
    unsigned int file_count = 0;
    unsigned int dir_count = 0;

    // Open directory
    dir = opendir(dirname);
    if (dir == NULL) {
        printf("There is no such directory: %s\n", dirname);
        return 1;
    }

    // Count files and dirs and save their names
    while ((item = readdir(dir)) != NULL) {
        strcat(path, item->d_name);  // Получаем полное имя файла
        stat(path, &stbuf);          // Получаем информацию о файле

        // File
        if (S_ISREG(stbuf.st_mode)) {
            if (file_count == max_file_count) {
                file_names = (char **)realloc(file_names, 2 * max_file_count * sizeof(char *));
                max_file_count *= 2;
            }

            // Push file name and its size
            file_names[file_count] = (char *)malloc(strlen(item->d_name) * sizeof(char *));
            strcpy(file_names[file_count], item->d_name);
            file_count++;

            // Directory
        } else if (S_ISDIR(stbuf.st_mode)) {
            if (strcmp(item->d_name, ".") != 0 && strcmp(item->d_name, "..") != 0) {
                if (dir_count == max_dir_count) {
                    dir_names = (char **)realloc(dir_names, 2 * max_dir_count * sizeof(char *));
                    max_dir_count *= 2;
                }

                // Push directory name
                dir_names[dir_count] = (char *)malloc(strlen(path) * sizeof(char *));
                strcpy(dir_names[dir_count], path);
                dir_count++;
            }
        }

        // Drop symbols after the last '/'
        path[dirname_len + 1] = '\0';
    }

    // Close directory
    closedir(dir);

    printf("%s%s%s\n", YELLOW_BOLD, dirname, NC);

    // Sort files
    qsort(file_names, file_count, sizeof(char *), cmpstringp);

    // Iterate over files
    for (unsigned int i = 0; i < file_count; i++) {
        strcat(path, file_names[i]);  // Get full file name
        stat(path, &stbuf);           // Get file info

        printf(" > %s%s  %s%lldB %s\n", YELLOW, file_names[i], RED, stbuf.st_size, NC);
        free(file_names[i]);

        path[dirname_len + 1] = '\0';
    }

    free(file_names);

    if (file_count > 0) {
        printf(" * There are %d files total\n\n", file_count);
    } else {
        printf(" * There is no any file\n\n");
    }

    // Sort directories
    qsort(dir_names, dir_count, sizeof(char *), cmpstringp);

    // Iterate over directories
    for (unsigned int i = 0; i < dir_count; i++) {
        show_dir(dir_names[i]);
        free(dir_names[i]);
    }

    free(dir_names);

    return 0;
}

int main(int argc, char *argv[]) {
    char dirname[PATH_MAX];
    struct stat stbuf;

    // Check input arguments
    switch (argc) {
        case 1:
            dirname[0] = '.';
            dirname[1] = '\0';
            break;

        case 2:
            memcpy(dirname, argv[1], strlen(argv[1]));
            dirname[strlen(argv[1])] = '\0';
            break;

        default:
            printf("Usage: %s [directory_name]\n", argv[0]);
            return 1;
    }

    // Remove unnececary '/' in the end of
    // the directory name
    if (dirname[strlen(dirname) - 1] == '/') {
        dirname[strlen(dirname) - 1] = '\0';
    }

    show_dir(dirname);

    return 0;
}
