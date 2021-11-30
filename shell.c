int main (int argc, char **argv){
    lsh_loop();

    return EXIT_SUCCESS;
}

void lsh_loo(void){
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
        position ++;

        //If the line takes up more space in memory than was allocated by the system, 
        // reallocate memory and loop back. If no such buffer exists, exit with error
        if (position>=bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if(!buffer){
                fprint(stderr, "lsh: allocation error \n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM "\t\r\n\a"
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
            bufsize += LSH_TOCK-BUFSIZE;
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
int lsh_launch(char **args){
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args ) == -1){
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {
        perror("lsh");
    }
    else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        
        }
        while (!WIFEXITED(status) && !WIFSIGNALLED(status));
    }
    return 1;
}