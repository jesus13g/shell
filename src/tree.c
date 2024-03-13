#include "builtin_tree.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RED     "\x1b[31m"
#define TAB           "   |"


int lsh_tree(char **args) {
    DIR *dir;
    char ruta[256] = ".";

    if (args[1] != NULL) {
        printf("-%s\n", args[1]);
    }
  
    if ((dir = opendir(ruta)) != NULL) {
        print_tree(ruta, 0);
        closedir(dir);
    } else {
        perror("No se pudo abrir el directorio");
        return 1;
    }

    return 1;
}

void print_tree(const char *ruta, int nivel) {
    DIR *dir = opendir(ruta);

    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    struct dirent *entry;
    char tab[50] = "";
    add_tab(tab, nivel);

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            struct stat info;
            char filepath[256];
            sprintf(filepath, "%s/%s", ruta, entry->d_name);
            if (stat(filepath, &info) == -1) {
                perror("Error al obtener información del archivo");
                continue;
            }
            
            if (S_ISREG(info.st_mode)) {    
                printf("%s", tab);
                printf(COLOR_GREEN"%s\n"COLOR_RESET, entry->d_name); 
            } else if (S_ISDIR(info.st_mode)) {   
                printf("%s", tab);
                printf(COLOR_BLUE"%s\n"COLOR_RESET, entry->d_name);
                int nuevo_nivel = nivel + 1;
                print_tree(filepath, nuevo_nivel);
            } else {
                printf("%s", tab);
                printf(COLOR_RED"%s (desconocido)\n"COLOR_RESET, entry->d_name);
            }
        }
    }

    closedir(dir);
}

void add_tab(char *tab, int nivel) {
    for (int i = 0; i < nivel; i++) {
        strcat(tab, TAB);
    }
}
