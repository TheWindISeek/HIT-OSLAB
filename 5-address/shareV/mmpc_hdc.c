#define __LIBRARY__
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>

#include <linux/sched.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t*, sem);
_syscall1(int, sem_post, sem_t*, sem);
_syscall1(int, sem_unlink, const char*, name);

_syscall2(int, shmget, key_t, key, size_t, size);
_syscall2(int, shmat, int, shmid, const void*, shmaddr);
_syscall1(int, shmdt, const void*, shmaddr);


#define NR_TASKS 5 
#define NR_ITEMS 20
#define BUFFER_SIZE 5 
#define FULL "full"
#define MUTEX "mutex"
#define EMPTY "empty"

int kill = 0;
void producer(int shmid, sem_t *full, sem_t *empty, sem_t *mutex) {
    unsigned int i;
    int pid = getpid();
	int pfull, pempty, pmutex;
    void *shared_memory = shmat(shmid, NULL);
    unsigned int *shared = (unsigned int*)shared_memory;

    for(i = 0; i < NR_ITEMS; ++i) {
        sem_wait(empty);
        sem_wait(mutex);

        shared[i%BUFFER_SIZE] = i; 

        printf("pid->%d\tproduces item->%d\n", pid,i);
        fflush(stdout);

        sem_post(mutex);
        sem_post(full);
    }   
    kill = 1;
    if(shmdt(shared_memory) == -1) {
        printf("%d producer shmdt failed\n", pid);
    }
}

void consumer(int shmid, sem_t *full, sem_t *empty, sem_t *mutex) {
    int pid = getpid();
    unsigned int i;
    printf("consumer%d have been created\n", pid);
    fflush(stdout);
    void* shared_memory = shmat(shmid, NULL);
    unsigned int *shared = (unsigned int*)shared_memory;
    
    while(!kill) {
        sem_wait(full);
        sem_wait(mutex);
        
        i = shared[shared[BUFFER_SIZE]]; 
        shared[BUFFER_SIZE] = (shared[BUFFER_SIZE]+1)%BUFFER_SIZE;

        printf("consumer\t%d consume %d\n", pid, i);
        fflush(stdout);
        sem_post(mutex);
        sem_post(empty);
    }
    printf("pid %d now dead\n", pid);
    fflush(stdout);
    
    if(shmdt(shared_memory) == -1) {
        printf("%d consumer shmdt failed\n", pid);
        fflush(stdout);
    }
}

int main (int argc, char* argv[]) {
    int i;
	sem_t *full, *empty, *mutex;
    int shmid = shmget((key_t)1234,
           (BUFFER_SIZE+1) * sizeof(unsigned int));
  
    full = sem_open(FULL, 0);
    empty = sem_open(EMPTY, BUFFER_SIZE);
    mutex = sem_open(MUTEX, 1);
   
    if(!fork()) {
        producer(shmid, full, empty, mutex);
        goto END;
    }

    for(i = 0; i < NR_TASKS; ++i) {
        if(!fork()) {
            consumer(shmid, full, empty, mutex);
            goto END;
        }
    }
    wait(&i);
END:
	sem_close(full);
	sem_close(empty);
	sem_close(mutex);
	sem_unlink(FULL);
	sem_unlink(EMPTY);
	sem_unlink(MUTEX);
    return 0;
}


