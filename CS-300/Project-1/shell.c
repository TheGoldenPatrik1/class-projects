#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// maximum length command
#define MAX_LINE 80

// define struct for our args
struct Arg {
    char *data[MAX_LINE/2 + 1];
    int should_wait;
    int output_redirect;
    char file_name[50];
};

int main(void) {
    // set up variables
    struct Arg args[10];
    int should_run = 1;

    // main while loop
    while (should_run) {
        // output prompt
        printf("crain_%d>", (int)getpid());
        fflush(stdout);

        // get line of input
        char str[MAX_LINE];
        if (!fgets(str, MAX_LINE, stdin)) break;
        
        // validate input
        size_t length = strlen(str);
        if (length == 0) break;
        if (str[length - 1] == '\n') str[length - 1] = '\0';
        
        // initialize variables
        char *token;                  
        token = strtok(str, " ");
        int argb = 0;
        int argc = 0;
        if (token == NULL) continue;
        args[argb].should_wait = 1;
        args[argb].output_redirect = 0;
        int is_input = 0;

        // read individual words from input line
        while (token != NULL) {
            // handle &
            if (strcmp(token, "&") == 0) {
                args[argb].should_wait = 0;
            // handle >
            } else if (strcmp(token, ">") == 0) {
                is_input = 1;
            // handle file output redirection
            } else if (is_input == 1) {
                strcpy(args[argb].file_name, token);
                is_input = 0;
                args[argb].output_redirect = 1;
            // handle &&
            } else if (strcmp(token, "&&") == 0) {
                args[argb].data[argc] = NULL;
                argc = 0;
                argb++;
                args[argb].should_wait = 1;
                args[argb].output_redirect = 0;
            // add word to arg list
            } else {
                args[argb].data[argc] = token;      
                argc++;
            }

            // get next word
            token = strtok(NULL, " ");
        }
        
        // set last arg to null
        args[argb].data[argc] = NULL; 
        argb++;
    
        // exit if necessary
        if (strcmp(args[0].data[0], "exit") == 0) {
            should_run = 0;
            continue;
        }

        // loop through args
        for (int i = 0; i < argb; i++) {
            // fork process
            pid_t pid = fork();
    
            // fork failed
            if (pid < 0) {
                // print error and exit
                fprintf(stderr, "Fork Failed\n");
                exit(1);
            // child process
            } else if (pid == 0) {
                // open file if necessary
                FILE *fp = NULL;
                if (args[i].output_redirect == 1) {
                    fclose(stdout);
                    fp = fopen(args[i].file_name, "w");
                }

                // execute command
                execvp(args[i].data[0], args[i].data);

                // close file if necessary
                if (fp != NULL) fclose(fp);
            // parent process
            } else {
                // wait if necessary - waitpid is needed to make the for loop work
                if (args[i].should_wait == 1) waitpid(pid, NULL, 0);
            }
        }
    }

    return 0;
}