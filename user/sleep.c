//sleep fuction
#include "kernel/types.h"
#include "user/user.h"

int atoi(const char *s);


void main(int argc, char *argv[]){
    if(argc == 1)
        printf("Error! Please input the number of ticks.\n");
    else if(argc == 2){
        int num;
        num = atoi(argv[1]);
        sleep(num);
    }
    else{
        printf("Error! You input too much argument or no argument.\n");
    }
    exit(0);
}