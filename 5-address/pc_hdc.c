#define __LIBRARY__
#include <unistd.h>
#include <linux/semaphore.h>
#include <stdio.h>

#include <linux/sched.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


_syscall2(sem_t*, sem_open, const char*, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t*, sem)
_syscall1(int, sem_post, sem_t*, sem)
_syscall1(int, sem_unlink, const char*, name)

#define NR_CONSUMERS 5
#define NR_ITEMS 20
#define BUFFER_SIZE 5 
const char* FILENAME = "pc.log";
int ckill = 0;
void producer(int file, sem_t *full, sem_t *empty, sem_t *mutex) {
    unsigned int i;
    int pid = getpid();
    for(i = 0; i < NR_ITEMS; ++i) {
        sem_wait(empty);
        sem_wait(mutex);
        if(!(i%BUFFER_SIZE)) {
            lseek(file, 0, SEEK_SET);
        }
        write(file, (char*)&i, sizeof(i)); 
        printf("pid->%d\tproduces item->%d\n", pid,i);
        fflush(stdout);

        sem_post(mutex);
        sem_post(full);
    }
    ckill = 1;
}

void consumer(int file, sem_t *full, sem_t *empty, sem_t *mutex) {
    int pid = getpid();
    unsigned int i;
    printf("consumer%d have been created\n", pid);
    while(!ckill) {
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
    filename = argc > 1 ? argv[1] : FILENAME;

    fi = open(FILENAME,O_WRONLY);
    fo = open(FILENAME,O_RDONLY);
    full = sem_open("full", 0);
    empty = sem_open("empty", BUFFER_SIZE);
    mutex = sem_open("mutex", 1);
    if(!fork()) {
		printf("now create producer process\n");
		fflush(stdout);
        producer(fi, full, empty, mutex);
        goto END;
    }
    for(i = 0; i < NR_CONSUMERS; ++i) {
        if(!fork()) {
            consumer(fo, full, empty, mutex);
            goto END;
        }
    }
    wait(&i);
END:
    close(fi);
    close(fo);
	sem_unlink("full");
    sem_unlink("empty");
	sem_unlink("mutex");
    printf("now say goodbye\n");
    return 0;
}


