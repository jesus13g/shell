#include <builtin_dir.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


int lsh_dir(char **args){

    if(args[1] == NULL){
        printf("Define el nombre de la carpeta\n");
        return 1;
    }

    if (mkdir(args[1], 0777) != 0) {
        printf("La carpeta \"%s\" no se ha podido crear.\n", args[0]);
    }

    return 1;
}