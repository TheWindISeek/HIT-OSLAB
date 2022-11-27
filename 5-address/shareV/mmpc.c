/* ************************************************************************
> File Name:     mmpc.c
> Author:        rebelOverWaist
> Description:   sem for producer, consumer question using shared memory for buf
 ************************************************************************/
#define __LIBRARY__
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>

#include <linux/sched.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

// for share memory
#include <sys/ipc.h>
#include <sys/shm.h>

/*
_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t*, sem);
_syscall1(int, sem_post, sem_t*, sem);
_syscall1(int, sem_unlink, const char*, name);

_syscall2(int, shmget, key_t, key, size_t, size);
_syscall2(int, shmat, int, shmid, const void*, shmaddr);
_syscall1(int, shmdt, const void*, shmaddr);

*/

#define NR_TASKS 5 /*how many tasks created*/
#define NR_ITEMS 20 /*how many items produced*/
#define BUFFER_SIZE 5 /*size of file buffer*/
// semaphores'name 
#define FULL "full"
#define MUTEX "mutex"
#define EMPTY "empty"

// used for kill consumer process
int kill = 0;


//Produce item put them into buffer 
// file WRONLY 
void producer(int shmid, sem_t *full, sem_t *empty, sem_t *mutex) {
    unsigned int i;
    int pid = getpid();
	int pfull, pempty, pmutex;
    //mapping shmid to current process and get the pointer
    void *shared_memory = shmat(shmid, NULL, 0);
    unsigned int *shared = (unsigned int*)shared_memory;

    /*get this value*/
    sem_getvalue(full,&pfull);
    sem_getvalue(empty,&pempty);
    sem_getvalue(mutex,&pmutex);    
    printf("full \tvalue=%d\n",pfull);
    printf("empty \tvalue=%d\n",pempty);
    printf("mutex \tvalue=%d\n",pmutex);   
    fflush(stdout);

    for(i = 0; i < NR_ITEMS; ++i) {
        sem_wait(empty);
        sem_wait(mutex);

        // if full then put from head
        shared[i%BUFFER_SIZE] = i; 

        printf("pid->%d\tproduces item->%d\n", pid,i);
        fflush(stdout);

        sem_post(mutex);
        sem_post(full);
    }   
	// producer have produced all items then kill all consumer
    kill = 1;
    if(shmdt(shared_memory) == -1) {
        printf("%d producer shmdt failed\n", pid);
    }
}


//consume item from buffer
//file RDONLY
void consumer(int shmid, sem_t *full, sem_t *empty, sem_t *mutex) {
    int pid = getpid();
    unsigned int i;
    printf("consumer%d have been created\n", pid);
    //mapping shmid to current process 
    void* shared_memory = shmat(shmid, NULL, 0);
    unsigned int *shared = (unsigned int*)shared_memory;
    
    while(!kill) {
        sem_wait(full);
        sem_wait(mutex);
        
        i = shared[shared[BUFFER_SIZE]]; 
        shared[BUFFER_SIZE] = (shared[BUFFER_SIZE]+1)%BUFFER_SIZE;

        printf("consumer\t%d consume %d\n", pid, i);
        sem_post(mutex);
        sem_post(empty);
    }
    printf("pid %d now dead\n", pid);
    fflush(stdout);
    
    if(shmdt(shared_memory) == -1) {
        printf("%d consumer shmdt failed\n", pid);
    }
}

int main (int argc, char* argv[]) {
    int i;
	sem_t *full, *empty, *mutex;
    int pfull, pempty, pmutex; 
    //get a shmget from 
    int shmid = shmget((key_t)1234,
           (BUFFER_SIZE+1) * sizeof(unsigned int)
            , 0666 | IPC_CREAT);
/*
    full = sem_open(FULL, 0);
    empty = sem_open(EMPTY, BUFFER_SIZE);
    mutex = sem_open(MUTEX, 1);
    */
    full=sem_open(FULL,O_CREAT,0666,0);
    empty=sem_open(EMPTY,O_CREAT,0666,BUFFER_SIZE);
	mutex = sem_open(MUTEX, O_CREAT,0666,1);
   
    if(full==(void*)-1 
        || empty==(void*)-1
        || mutex==(void*)-1){
        perror("sem_open failure");
    }
    // get this value
    sem_getvalue(full,&pfull);
    sem_getvalue(empty,&pempty);
    sem_getvalue(mutex,&pmutex);    
   /* printf("full \tvalue=%d\n",pfull);
    printf("empty \tvalue=%d\n",pempty);
    printf("mutex \tvalue=%d\n",pmutex);  
	*/
    //create producer process
    if(!fork()) {
        producer(shmid, full, empty, mutex);
        goto END;
    }

    //create consumer process
    for(i = 0; i < NR_TASKS; ++i) {
        if(!fork()) {
            consumer(shmid, full, empty, mutex);
            goto END;
        }
    }
    //wait all subprocess exit
    wait(&i);
END:
    //release resoureces
	sem_close(full);
	sem_close(empty);
	sem_close(mutex);
	sem_unlink(FULL);
	sem_unlink(EMPTY);
	sem_unlink(MUTEX);
	//
    printf("now say goodbye\n");
    return 0;
}


