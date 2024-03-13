#include "lsh.h"
#include "builtin_cd.h"
#include "builtin_help.h"
#include "builtin_exit.h"
#include "builtin_path.h"
#include "builtin_ls.h"
#include "builtin_tree.h"
#include "builtin_dir.h"
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET   "\x1b[0m"
#define TAB           "   |"

int lsh_launch(char **args);
int lsh_num_builtins();

char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "path",
  "ls",
  "tree",
  "dir"
};

int (*builtin_func[])(char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &lsh_path,
  &lsh_ls,
  &lsh_tree,
  &lsh_dir
};

void lsh_loop(void) {
  char *line;
  char **args;
  int status;
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));

  do {
    printf(COLOR_BLUE"%s> "COLOR_RESET,cwd);
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

char *lsh_read_line(void) {
  char *line = NULL;
  size_t bufsize = 0; 

  if (getline(&line, &bufsize, stdin) == -1){
    if (feof(stdin)) {
      exit(EXIT_FAILURE);
    } else  {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}

char **lsh_split_line(char *line) {
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = strdup(token);
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  
  return tokens;
}

int lsh_launch(char **args) {
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("lsh");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int lsh_execute(char **args) {
  int i;

  if (args[0] == NULL) {
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}
