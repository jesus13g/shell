#include <builtin_delete.h>
#include <stdio.h>
#include <stdlib.h>

int lsh_delete(char **args){

    if(args[1] == NULL){
        printf("Falta definir el nombre.\n");
        return 1;
    }

    if(remove(args[1]) != 0){
        printf("Error al eliminar %s.\n", args[1]);
    }

    return 1;
}