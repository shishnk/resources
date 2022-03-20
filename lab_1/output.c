#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define CYAN "\033[0;36m"
#define CLEAR "\033[0m"

void show_dir(const char *path) {
    DIR *dir;
    struct stat info;
    struct dirent *item;
    struct tm *timeinfo;
    char file_path[PATH_MAX];
    char buff_time[20];
    unsigned int file_num = 0;

    dir = opendir(path);

    if (dir == NULL) {
        printf("%sПапка: %s%s%s не существует.%s\n", YELLOW, BLUE, path, YELLOW, CLEAR);
        exit(0);
    } else
        printf("%sПапка: %s%s %s\n", YELLOW, BLUE, path, CLEAR);

    strcpy(file_path, path);

    while ((item = readdir(dir)) != NULL) {
        strcat(file_path, "/");
        strcat(file_path, item->d_name);
        stat(file_path, &info);

        if (S_ISREG(info.st_mode))
            file_num++;

        strcpy(file_path, path);
    }

    closedir(dir);
    dir = opendir(path);

    char files_names[file_num][255];
    file_num = 0;

    while ((item = readdir(dir)) != NULL) {
        strcat(file_path, "/");
        strcat(file_path, item->d_name);
        stat(file_path, &info);

        if (S_ISREG(info.st_mode)) {
            strcpy(files_names[file_num], item->d_name);
            file_num++;
        }

        strcpy(file_path, path);
    }

    qsort(files_names, file_num, NAME_MAX, strcmp);

    for (int i = 0; i < file_num; i++) {
        strcat(file_path, "/");
        strcat(file_path, files_names[i]);
        stat(file_path, &info);

        timeinfo = localtime(&info.st_mtime);
        strftime(buff_time, 20, "%b %d %H:%M", timeinfo);

        printf(" * %s%s: %s%d bytes, modification date: %s%s\n", GREEN, files_names[i], RED, info.st_size, buff_time, CLEAR);

        strcpy(file_path, path);
    }

    printf("%sОбщее количество файлов: %s%d%s\n", CYAN, RED, file_num, CLEAR);

    closedir(dir);
    dir = opendir(path);

    while ((item = readdir(dir)) != NULL) {
        strcat(file_path, "/");
        strcat(file_path, item->d_name);
        stat(file_path, &info);

        if (S_ISDIR(info.st_mode)) {
            if (strcmp(item->d_name, ".") != 0 &&
                strcmp(item->d_name, "..") != 0) {
                show_dir(file_path);
            }
        }

        strcpy(file_path, path);
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    char path[PATH_MAX];

    switch (argc) {
        case 1:
            path[0] = '.';
            path[1] = '\0';
            break;

        case 2:
            memcpy(path, argv[1], strlen(argv[1]));
            path[strlen(argv[1])] = '\0';
            break;

        default:
            printf("%sВвод:%s %s %s[directory]%s\n", YELLOW, CLEAR, argv[0], BLUE, CLEAR);
            exit(0);
    }

    show_dir(path);

    return 0;
}