/* ************************************************************************
> File Name:     pc.c
> Author:        rebelOverWaist
> Description:   sem for producer, consumer question 
 ************************************************************************/
#define __LIBRARY__
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>

#include <linux/sched.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t*, sem);
_syscall1(int, sem_post, sem_t*, sem);
_syscall1(int, sem_unlink, const char*, name);
*/

#define NR_TASKS 5 /*how many tasks created*/
#define NR_ITEMS 20 /*how many items produced*/
#define BUFFER_SIZE 5 /*size of file buffer*/
// semaphores'name 
#define FULL "full"
#define MUTEX "mutex"
#define EMPTY "empty"

// this is the file's name used for buffer
const char* FILENAME = "pc.log";
// used for kill consumer process
int kill = 0;

//Produce item put them into buffer
// file WRONLY 
void producer(int file, sem_t *full, sem_t *empty, sem_t *mutex) {
    unsigned int i;
    int pid = getpid();
	int pfull, pempty, pmutex;
    /*get this value*/
    sem_getvalue(full,&pfull);
    sem_getvalue(empty,&pempty);
    sem_getvalue(mutex,&pmutex);    
    printf("full \tvalue=%d\n",pfull);
    printf("empty \tvalue=%d\n",pempty);
    printf("mutex \tvalue=%d\n",pmutex);   
    for(i = 0; i < NR_ITEMS; ++i) {
        sem_wait(empty);
        sem_wait(mutex);

        // if full then put from head
        if(!(i%BUFFER_SIZE)) {
            lseek(file, 0, SEEK_SET);
        }
        write(file, (char*)&i, sizeof(i)); 
        printf("pid->%d\tproduces item->%d\n", pid,i);
        fflush(stdout);

        sem_post(mutex);
        sem_post(full);
    }   
	// producer have produced all items then kill all consumer
    kill = 1;
}


//consume item from buffer
//file RDONLY
void consumer(int file, sem_t *full, sem_t *empty, sem_t *mutex) {
    int pid = getpid();
    unsigned int i;
    printf("consumer%d have been created\n", pid);
    while(!kill) {
        sem_wait(full);
        sem_wait(mutex);
        
        if(!read(file, (char*)&i, sizeof(i))) {
            lseek(file, 0, SEEK_SET);
            read(file, (char*)&i, sizeof(i));
        }
        printf("consumer\t%d consume %d\n", pid, i);
        sem_post(mutex);
        sem_post(empty);
    }
    printf("pid %d now dead\n", pid);
    fflush(stdout);
}

int main (int argc, char* argv[]) {
    int i;
	int fi, fo;
	int pfull, pempty, pmutex;
    char* filename;
	sem_t *full, *empty, *mutex;
	// if have cmd arg then use it, otherwise use default path
    filename = argc > 1 ? argv[1] : FILENAME;

    fi = open(FILENAME,O_WRONLY | O_TRUNC, 0666);
    fo = open(FILENAME,O_RDONLY | O_TRUNC, 0666);
    // create sem 
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
		printf("now create producer process\n");
		fflush(stdout);
        producer(fi, full, empty, mutex);
        goto END;
    }

    //create consumer process
    for(i = 0; i < NR_TASKS; ++i) {
        if(!fork()) {
            consumer(fo, full, empty, mutex);
            goto END;
        }
    }
    //wait all subprocess exit
    wait(&i);
END:
    //release resoureces
    close(fi);
    close(fo);
	// 
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


