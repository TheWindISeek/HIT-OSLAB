/* ************************************************************************
> File Name:     test.c
> Author:        程序员Carl
> 微信公众号:    代码随想录
> Created Time:  2022年11月17日 星期四 15时10分54秒
> Description:   
 ************************************************************************/
// should use gcc -o outputFileName fileName -lpthread
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

void newprocess() {

}

int main () {
    FILE* file = fopen("pc.log", "w");
    
    fclose(file);
    sem_t f;
    if(!fork()) {
        newprocess();
        return 0;
    }

    sem_init(&f, 0, 0);
    sem_destroy(&f);
    return 0;
}

