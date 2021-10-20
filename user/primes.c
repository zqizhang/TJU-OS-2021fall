//primes fuction
#include "kernel/types.h"
#include "user/user.h"

int num = 35;

//埃氏筛法寻找质数
void primes(int *array, int num){
    if(num == 1){
        printf("prime %d\n", *array);
        return ;
    }
    int fd[2];
    if(pipe(fd) == -1){
        printf("Fail to create pipe\n");
        return ;
    }
    int base, temp;
    base = *array;
    printf("prime %d\n", base);

    if(fork() == 0){
        int i;
        for(i = 0; i < num; ++i){
            temp = array[i];
            write(fd[1], &temp, sizeof(int));
        }
        exit(0);
    }

    close(fd[1]);

    if(fork() == 0){
        int ans = 0;
        while(read(fd[0], &temp, sizeof(int)) != 0){
            //每读出一段数据，被读出的数据会被从pipe中清除
            if(temp % base != 0){
                *array = temp;
                array += 1;
                ans++;
            }
        }
        primes(array - ans, ans);
        exit(0);
    }

    wait(0);
}

void main(int argc, char* argv){
    int array[num - 1];;
    int i;
    for(i = 0; i < num - 1; ++i){
        array[i] = i + 2;
    }
    primes(array, num - 1);

    exit(0);
}

