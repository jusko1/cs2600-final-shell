//sys/wait has error because it is not supported on windows machines?, need linux
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//takes the arguments from the split() method 
int lsh_launch(char **args){
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        //takes in a program  name and an array
        //if it returns, there was an error
        if (execvp(args[0], args) == -1){
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }
    //another error
    else if (pid < 0) {
        perror("lsh");
    }
    else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        }
        while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}
//need to declare here 
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

//holds the built in functions
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};
//so that we can use them here
int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

// returns the number of built in functions by doing math on 
// the size of the struct and the size of a char
int lsh_num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}

//function for changing directory
int lsh_cd(char **args){
    //no argument given, returns an error
    if (args[1] == NULL) {
        fprintf(stderr, "ish: expected argument to \"cd\"\n");
    }
    // if there is an argument, calls chdir()
    else {
        // if there is an error, returns an error
        if (chdir(args[1]) != 0){
            perror("lsh");
        }
    }
    return 1;
}
//prints the built in functions by using the struct from earlier
int lsh_help(char **args){
    int i;
    printf("An LSH from Brennan\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < lsh_num_builtins(); i++){
        printf(" %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}
//exiting the shell
int lsh_exit(char **args){
    return 0;
}

int lsh_execute(char **args){
    int i;
    
    if (args[0] == NULL){
        return 1;
    }
    //cycles through the 3 builtin options
    for (i = 0; i < lsh_num_builtins(); i++){
        //if the string matches one of the builtin functions, 
        if (strcmp(args[0], builtin_str[i]) == 0) {
            //runs the function
            return (*builtin_func[i])(args);
        }
    }
    return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void)
{
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c; 

    if (!buffer) {
        fprintf(stderr, "lsh: allocatin error\n");
        exit(EXIT_FAILURE);
    }
    while (1){
        c = getchar();

        //As long as the char is not the end of file or a newline character,
        // if it is one of the two, return the string
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        // add the character the the existing string
        else {
            buffer[position] = c;
        }
        position++;
        //If the line takes up more space in memory than was allocated by the system, 
        // reallocate memory and loop back. If no such buffer exists, exit with error
        if (position>=bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if(!buffer){
                fprintf(stderr, "lsh: allocation error \n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
//splits the line into a list of arguments using whitespace as delimiters
char **lsh_split_line(char *line){
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens){
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line, LSH_TOK_DELIM);
    //while the token is not null, adds it to the array
    while(token != NULL) {
        tokens[position] = token;
        position++;
        // if the memory allocated is not enough, reallocate more memory
        if (position >= bufsize){
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(token, bufsize * sizeof(char*));
            if (!tokens) {
                //if memory reallocation fails, prints error and exits.
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        //go to the next token
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    //the array is null-terminated
    tokens[position] = NULL;
    //returns an array of tokens
    return tokens;
}
void lsh_loop(void){
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
    }
    while (status);
}

int main (int argc, char **argv){
    lsh_loop();

    return EXIT_SUCCESS;
}
