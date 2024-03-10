#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET   "\x1b[0m"
#define TAB           "   |"

void lsh_loop(void);
char *lsh_read_line(void);
char **lsh_split_line(char *line);
int lsh_execute(char **args);
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_path(char **args);
int lsh_ls(char **args);
int lsh_tree(char **args);
void print_files_in_columns(DIR *dir, struct winsize w);
void print_tree(const char *ruta, int nivel);
void add_tab(char *tab, int nivel);
//int lsh_(char **args);

char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "path",
  "ls",
  "tree"
};

int (*builtin_func[])(char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &lsh_path,
  &lsh_ls,
  &lsh_tree
};

int main(int argc, char **argv) {
  // Load config files, if any.

  // Run command loop.
  lsh_loop();
  // Perform any shutdown/cleanup.

  return 0;
}

void lsh_loop(void) {
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

char *lsh_read_line(void) {
  char *line = NULL;
  size_t bufsize = 0; // have getline allocate a buffer for us

  if (getline(&line, &bufsize, stdin) == -1){
    if (feof(stdin)) {
      exit(EXIT_FAILURE);  // We recieved an EOF
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
    // Proceso hijo
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Proceso padre
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
int lsh_(char **args){

  return 1;
}
*/

int lsh_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char **args) {
  int i;
  printf("Comandos posibles\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  return 1;
}

int lsh_exit(char **args) {
  return 0;
}

int lsh_path(char **args){
  char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("path no encontrado");
    }


  return 1;
}



int lsh_ls(char **args){
  DIR *dir;
  char ruta[256] = ".";

  if(args[1] != NULL){
    printf("-%s\n",args[1]);
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
          current_width += filename_length + 2; // deja 2 espacios a la derecha
          if (current_width >= max_width) {
              printf("\n");
              current_width = 0;
          }
      }
  }
  printf("\n");
}


int lsh_tree(char **args){
  DIR *dir;
  char ruta[256] = ".";

  if(args[1] != NULL){
    printf("-%s\n",args[1]);
  }
  
  if ((dir = opendir(ruta)) != NULL) {
    print_tree(ruta,0);
    closedir(dir);
  } else {
    perror("No se pudo abrir el directorio");
    return 1;
  }

  return 1;
}


/*
  Verde = archivos
  Azul = Carpetas
*/
void print_tree(const char *ruta, int nivel) {
    // Abrir el directorio proporcionado
    DIR *dir = opendir(ruta);

    // Verificar si se pudo abrir el directorio
    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    struct dirent *entry;
    char tab[50] = "";
    add_tab(tab,nivel);

    // Leer cada entrada del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar las entradas "." y ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // Obtener la información del archivo
            struct stat info;
            char filepath[256];
            sprintf(filepath, "%s/%s", ruta, entry->d_name);
            if (stat(filepath, &info) == -1) {
                perror("Error al obtener información del archivo");
                continue;
            }
            
            
            if (S_ISREG(info.st_mode)) {    // Comprobar si es un archivo regular
                printf("%s", tab);
                printf(COLOR_GREEN"%s\n"COLOR_RESET, entry->d_name); 
            } else if (S_ISDIR(info.st_mode)) {   // Comprobar si es un directorio
                printf("%s", tab);
                printf(COLOR_BLUE"%s\n"COLOR_RESET, entry->d_name);
                int nuevo_nivel = nivel + 1;
                print_tree(filepath,nuevo_nivel);
            } else {
                printf("%s", tab);
                printf(COLOR_RED"%s (desconocido)\n"COLOR_RESET, entry->d_name);
            }
        }
    }

    // Cerrar el directorio
    closedir(dir);
}

void add_tab(char *tab,int nivel){
  for (int i = 0; i < nivel; i++) {
        strcat(tab, TAB);
    }
}

int lsh_execute(char **args) {
  int i;

  if (args[0] == NULL) {  //Si no hay entrada lanza de nuevo
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}
