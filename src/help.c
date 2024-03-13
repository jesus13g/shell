#include "builtin_help.h"
#include <stdio.h>

int lsh_help(char **args) {
  printf("Comandos posibles:\n");
  printf("  cd\n");
  printf("  help\n");
  printf("  exit\n");
  printf("  path\n");
  printf("  ls\n");
  printf("  tree\n");
  return 1;
}
