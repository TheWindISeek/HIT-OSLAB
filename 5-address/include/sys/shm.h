/* ************************************************************************
> File Name:     include/linux/shm.h
> Author:       
> Description:   
 ************************************************************************/
#ifndef __SHM__
#define __SHM__

#define SHM_SIZE 8
typedef unsigned int key_t;

typedef struct shm {
    //key to shm
    key_t key;
    //size of shared memory
    size_t size;
    //address of physical memory
    unsigned long page;
    //address of current process
    unsigned int count;
}shm;

int sys_shmget(key_t key, size_t size);
void *sys_shmat(int shmid, void *shmaddr);
int sys_shmdt(void *shmaddr);

#endif
