#include "builtin_path.h"
#include <stdio.h>
#include <unistd.h>

int lsh_path(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("path no encontrado");
    }
    return 1;
}
