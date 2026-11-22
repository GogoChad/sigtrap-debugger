#include <stdio.h>
void welcome(void);
void status(int pid);

int main(int argc, char **argv){
    int counter = 1;
    welcome();
    status(10);
    printf("arguments number : %d\n", argc);
    printf("Program name : %s\n", argv[0]);
    if (argc == 1){
        printf("An error has occured, please input a target program.\n");
    }
    else if (argc >= 2){
        printf("Target program : %s\n", argv[1]);
    }
    while (counter == 1){
        printf("SIGTRAP is running\n");
        counter = 0;
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
