/* ************************************************************************
> File Name:     address.c
> Author:        rebelOverWaist 
> Created Time:  2022年11月26日 星期六 11时07分23秒
> Description:   
 ************************************************************************/
#include <stdio.h>

int i = 0x12345678;

int main (void) {
    printf("The logical/virtual address of i is 0x%08x", &i);
    fflush(stdout);

    while(i)
        ;

    return 0;
}
