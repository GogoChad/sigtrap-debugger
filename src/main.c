#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "process.h"

void welcome(void);
void help(void);

int main(void){
    char command[30];
    const char *prompt = isatty(STDOUT_FILENO) ? "\033[31msigtrap>\033[0m " : "sigtrap> ";
    welcome();
    while (1)
    {
        fputs(prompt, stdout);
        fflush(stdout);
    if (fgets(command, sizeof(command), stdin) == NULL){
        printf("End of stream.\n");
        break;
    }
    else{
        size_t lenght = strcspn(command, "\n");
        command[lenght] = '\0';
        if(strcmp("help", command) == 0){
            help();
        }
        else if (strcmp("quit", command) == 0){
            printf("sigtrap exit successfully.");
            break;
        }
        else if (strcmp("",command) == 0){
            continue;
        }
        else if (strcmp("run",command) == 0){
            printf("Use run ./target\n");
        }
        else if (strncmp("run ",command, 4) == 0){
            if (command[4] == '\0') {
                printf("Input a target file.\n");
            } else {
                run_program(&command[4]);
            }

        }
        else if (strcmp("debug", command) == 0){
            printf("Use debug ./target\n");
        }
        else if (strncmp("debug ", command, 6) == 0){
            if (command[6] == '\0') {
                printf("Input a target file.\n");
            } else {
                debug_program(&command[6]);
            }
        }
        else if (strcmp("continue", command) == 0){
            continue_program();
        }
        else if (strcmp("step", command) == 0){
            step_program();
        }
        else{
            printf("Unknown command. Type help.\n");
        }
    }
    }
    close_debug_session();
}

void welcome(void){
    printf("Welcome to sigtrap debugger!\n");
}

void help(void){
    printf("help - show all commands\n"
           "run <path> - run a program\n"
           "debug <path> - start a program under the debugger\n"
           "continue - resume the stopped program\n"
           "step - execute one machine instruction\n"
           "quit - exit SIGTRAP\n");
}
