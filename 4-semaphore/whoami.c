/* ************************************************************************
> File Name:     whoami.c
> Author:        rebelOverWaist
> Created Time:  2022年11月05日 星期六 18时30分35秒
> Description:  
 ************************************************************************/
#define __LIBRARY__
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

_syscall2(int, whoami, char*, name, unsigned int, size)
#define SIZE 23
int main (void) {
    char name[SIZE + 1];
    int res;
    res = whoami(name, SIZE+1);
    if(res == -1) {
        errno = EINVAL;
     } else {
        printf("%s\n", name);
     }
    return res;
}

