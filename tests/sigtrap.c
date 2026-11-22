#include <stdio.h>
void welcome(void);
void status(int pid);

int main(void){
    char command[100];
    printf("Input a command:\n");
    if (fgets(command, sizeof(command), stdin) == NULL){
        printf("This argument is not valid, please retry or chose a different argument.\n");
    }
    else {
    printf("Input command: %s",command);
    }
    return 0;
}

void welcome(void){
    printf("Welcome to sigtrap debugger!\n");
}

void status(int pid){
    if (pid == 0) {
        printf("nothing is attached yet\n");
    }
    else if (pid > 0){
        printf("Process attached : PID %d\n",pid);
    }
    else {
        printf("Fatal Error\n");
    }
}
