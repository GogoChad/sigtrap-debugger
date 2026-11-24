#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void welcome(void);
void help(void);

int main(void){
    char command[30];
    welcome();
    while (1)
    {
        printf("sigtrap> ");
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
                const char *path = &command[4];
                pid_t pid = fork();
                if (pid == -1){
                    perror("fork");
                }
                else if (pid == 0){
                    execl(path, path, (char *) NULL);
                    perror("execl");
                    _exit(1);
                }
                else{
                    if (waitpid(pid, NULL, 0) == -1){
                        perror("waitpid");
                    }
                }
            }

        }
        else{
            printf("Unknown command. Type help.\n");
        }
    }
    }
    

}

void welcome(void){
    printf("Welcome to sigtrap debugger!\n");
}

void help(void){
    printf("help - show all commands\nquit - exit SIGTRAP\n");
}