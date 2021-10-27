#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Error! You input too few arguments.\n");
        exit(0);
    }
    if(argc + 1 > MAXARG) {
        printf("Error! You input too many arguments.\n");
        exit(0);
    }
    char *command = argv[1];
    char *argument[MAXARG], buf[512];
    int i;

    for (i = 1; i < argc; i++) {
        argument[i - 1] = argv[i];
    }
    argument[argc] = 0;

    while(1) {
        i = 0;
        while(1) {
            int n = read(0, &buf[i], 1);
            if(n == 0 || buf[i] == '\n') 
                break;
            i++;
        }
        if(i == 0) 
            break;
        buf[i] = 0;
        argument[argc - 1] = buf;
        if(fork() == 0) {
            exec(command, argument);
            exit(0);
        } 
        else{
            wait(0);
        }
    }
    exit(0);
}
