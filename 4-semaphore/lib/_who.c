/* ************************************************************************
> File Name:     _who.c
> Author:        rebelOverWaist
> Created Time:  2022-11-5 12:58
> Description:   API of _who
 ************************************************************************/

#define __LIBRARY__ //access _syscall1 && _syscall2
#include <unistd.h> // know the nr of iam and nr of whoami
//API in user'space
_syscall1(int, iam, const char*, name);
_syscall2(int, whoami, char*, name, unsigned int, size);

