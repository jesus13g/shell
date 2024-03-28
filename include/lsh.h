#ifndef LSH_H
#define LSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

void lsh_loop(void);
char *lsh_read_line(void);
char **lsh_split_line(char *line);
int lsh_execute(char **args);
int lsh_launch(char **args);
int lsh_num_builtins();
char **lsh_completion(const char *text, int start, int end);

#endif
