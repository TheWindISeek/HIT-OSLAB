/* ************************************************************************
> File Name:     pc.c
> Author:        rebelOverWaist
> Description:   sem for producer, consumer question 
 ************************************************************************/
#define __LIBRARY__
#include <unistd.h>
// #include <linux/sem.h>
#include <semaphore.h>
#include <stdio.h>

#include <linux/sched.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


#define BUFFER_SIZE 10

/*
_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t*, sem);
_syscall1(int, sem_post, sem_t*, sem);
_syscall1(int, sem_unlink, const char*, name);
*/
const char* FILENAME = "pc.log";
sem_t full, empty;
sem_t access_mutex, print_mutex;
int kill = 0;
int fi, fo;

//Produce item put them into buffer
void producer(int file) {
    unsigned int M = 500, i;
    int pid = getpid();
    for(i = 0; i < M; ++i) {
        // P(empty) 
        sem_wait(&empty);
        // P(access_mutex)
        sem_wait(&access_mutex);
        // put item
        // if full then put from head
        if(!(i%BUFFER_SIZE)) {
            lseek(file, 0, SEEK_SET);
        }
        write(file, (char*)&i, sizeof(i)); 
        printf("pid->%d\tproduces item->%d\n", pid,i);
        fflush(stdout);
        // V(access_mutex)
        sem_post(&access_mutex);
        
        // V(full)
        if(sem_post(&full) == 0) {
            printf("\nyou p full\n");
            fflush(stdout);
        } else {
            printf("\nfailed to p full\n");
            fflush(stdout);
        }
    }   
    kill = 1;
}

//consume item from buffer
void consumer(int file) {
    int pid = getpid();
    unsigned int i;
    printf("consumer%d have been created\n", pid);
    while(!kill) {
        printf("consumer%d now wait full\n", pid);
        fflush(stdout);
        // P(full)
        while(sem_wait(&full) == -1) {
            printf("consumer full\n");
        }
        printf("consumer%d now wait access_mutex\n", pid);
        fflush(stdout);
        // P(access_mutex)
        sem_wait(&access_mutex);
        //
        printf("consumer%d now read \n", pid);
        fflush(stdout);
        if(!read(file, (char*)&i, sizeof(i))) {
            lseek(file, 0, SEEK_SET);
            read(file, (char*)&i, sizeof(i));
        }
        
        // V(access_mutex)
        sem_post(&access_mutex);
        printf("consumer%d now post access_mutex\n", pid);
        fflush(stdout);
        // V(empty)
        sem_post(&empty);
    }
    printf("pid %d now dead\n", pid);
    fflush(stdout);
}

int main (int argc, char* argv[]) {
    int i;

    //create buffer 
    char* filename;
    filename = argc > 1 ? argv[1] : FILENAME;
    fi = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0222);
    fo = open(filename, O_TRUNC | O_RDONLY, 0444);
    // create sem 
    /*
    full = sem_open("full", 0);
    empty = sem_open("empty", BUFFER_SIZE);
    access_mutex = sem_open("access_mutex", 1);
    print_mutex = sem_open("print_mutex", 1);
    */

    printf("full inti \t%d\n", sem_init(&full, 1, 0));  
    printf("empty init \t%d\n", sem_init(&empty, 1, BUFFER_SIZE));
    printf("access_mutex init\t%d\n", sem_init(&access_mutex, 1, 1));
    printf("print_mutex init \t%d\n",sem_init(&print_mutex, 1, 1));
    
    printf("create producer process\n");
    //create producer process
    if(!fork()) {
        producer(fi);
        return 0;
    }
    printf("create consumer process\n");
    //create consumer process
    for(i = 0; i < 10; ++i) {
        if(!fork()) {
            consumer(fo);
            return 0;
        }
    }
    printf("wait all subprocess exit\n");
    //wait all subprocess exit
    wait(&i);
    //relese all uesd resoureces
    close(fi);
    close(fo);
    /*
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("access_mutex");
    sem_unlink("print_mutex");
    */
    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&access_mutex);
    sem_destroy(&print_mutex);
    printf("now say goodbye\n");
    return 0;
}


