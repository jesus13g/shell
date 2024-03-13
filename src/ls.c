#include "builtin_ls.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/ioctl.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_BLUE    "\x1b[34m"

void print_files_in_columns(DIR *dir, struct winsize w);

int lsh_ls(char **args) {
    DIR *dir;
    char ruta[256] = ".";

    if(args[1] != NULL){
        printf("-%s\n", args[1]);
    }
  
    if ((dir = opendir(ruta)) != NULL) {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        print_files_in_columns(dir, w);
        closedir(dir);
    } else {
        perror("No se pudo abrir el directorio");
        return 1;
    }

    return 1;
}

void print_files_in_columns(DIR *dir, struct winsize w) {
    int max_width = w.ws_col - 5;
    int current_width = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            int filename_length = printf("%s  ", entry->d_name);
            current_width += filename_length + 2; // Añade 2 espacios a la derecha
            if (current_width >= max_width) {
                printf("\n");
                current_width = 0;
            }
        }
    }
    printf("\n");
}
