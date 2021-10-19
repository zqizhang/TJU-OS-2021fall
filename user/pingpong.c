//pingpong fuction
#include "kernel/types.h"
#include "user/user.h"
// #include "kernel/pipe.c"

void main(int argc, char* argv){
    int fd_pa[2], fd_ch[2];//written by parent and child
    char buf[5];
    if(pipe(fd_pa) == -1 || pipe(fd_ch) == -1){
        printf("Fail to create pipe\n");
    }
    if(fork() > 0){
        write(fd_pa[1], "ping", strlen("ping"));
        read(fd_ch[0], buf, strlen("pong"));
        printf("%d: received %s\n", getpid(), buf);
    }
    else if(fork() == 0){
        read(fd_pa[0], buf, strlen("ping"));
        printf("%d: received %s\n", getpid(), buf);
        write(fd_ch[1], "pong", strlen("ping"));
    }
    else if(fork() == -1){
        printf("Fail to create process\n");
    }
    exit(0);
}