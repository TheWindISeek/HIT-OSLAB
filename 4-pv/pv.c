#include <fcntl.h>           
#include <sys/stat.h>        
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>


#define BUFFER_SIZE 5
#define FILENAME "something"
#define NR_TASKS 10
#define NR_ITEMS 20
#define FULL "full"
#define EMPTY "empty"
#define MUTEX "mutex"

int kill = 0;
//Produce item put them into buffer
void producer(int file, sem_t* empty, sem_t* mutex, sem_t* full) {
    unsigned int i;
    int pid = getpid();
	int pmutex, pempty, pfull;
    for(i = 0; i < NR_ITEMS; ++i) {
        // P(empty) 
        sem_wait(empty);
        // P(access_mutex)
        sem_wait(mutex);
        // put item
        // if full then put from head
        if(!(i%BUFFER_SIZE)) {
            lseek(file, 0, SEEK_SET);
        }
        write(file, (char*)&i, sizeof(i)); 
        printf("pid->%d\tproduces item->%d\n", pid,i);
        fflush(stdout);
        // V(access_mutex)
        sem_post(mutex);
        
        // V(full)
        sem_post(full);
    /*get this value*/
    sem_getvalue(full,&pfull);
    sem_getvalue(empty,&pempty);
    sem_getvalue(mutex,&pmutex);    
    printf("full \tvalue=%d\n",pfull);
    printf("empty \tvalue=%d\n",pempty);
    printf("mutex \tvalue=%d\n",pmutex);
fflush(stdout);
    }   
    kill = 1;
}

//consume item from buffer
void consumer(int file, sem_t* full, sem_t* empty, sem_t* mutex) {
    int pid = getpid();
    unsigned int i;
    printf("consumer%d have been created\n", pid);
    while(!kill) {
        printf("consumer%d now wait full\n", pid);
        fflush(stdout);
        // P(full)
        sem_wait(full);
        printf("consumer%d now wait access_mutex\n", pid);
        fflush(stdout);
        // P(access_mutex)
        sem_wait(mutex);
        //
        printf("consumer%d now read \n", pid);
        fflush(stdout);
        if(!read(file, (char*)&i, sizeof(i))) {
            lseek(file, 0, SEEK_SET);
            read(file, (char*)&i, sizeof(i));
        }
        
        // V(access_mutex)
        sem_post(mutex);
        printf("consumer%d now post access_mutex\n", pid);
        fflush(stdout);
        // V(empty)
        sem_post(empty);
    }
    printf("pid %d now dead\n", pid);
    fflush(stdout);
}
int main(int argc,char *argv[]){
    /* create the named pipe fifo*/
    int fi, fo;
    fi = open(FILENAME,O_WRONLY);
    fo = open(FILENAME,O_RDONLY);
    /*open the semaphore*/
    sem_t *full,*empty, *mutex;
    int pfull, pempty, pmutex;
    full=sem_open(FULL,O_CREAT,0666,0);
    empty=sem_open(EMPTY,O_CREAT,0666,BUFFER_SIZE);
	mutex = sem_open(MUTEX, O_CREAT,0666,1);
    if(full==(void*)-1 
        || empty==(void*)-1
        || mutex==(void*)-1){
        perror("sem_open failure");
    }
	//print sem address
    printf("sem address\n");
    printf("full=%p\n",full);
    printf("empty=%p\n",empty);
    printf("mutex=%p\n",mutex);

    /*get this value*/
    sem_getvalue(full,&pfull);
    sem_getvalue(empty,&pempty);
    sem_getvalue(mutex,&pmutex);    
    printf("full \tvalue=%d\n",pfull);
    printf("empty \tvalue=%d\n",pempty);
    printf("mutex \tvalue=%d\n",pmutex);   
    if(!fork()) {
		producer(fi, empty, mutex, full);
		goto END;
	}
	int i;
	for(i = 0; i < NR_TASKS; ++i) {
		if(!fork()) {
			consumer(fo, full, empty, mutex);
			goto END;		
		}
	}
	wait(&i);
END:
    /*close the file*/
    close(fi);
    close(fo);
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    /* release resource*/
    unlink(FILENAME);
    sem_unlink(EMPTY);
    sem_unlink(MUTEX);
    sem_unlink(FULL);
    return 0;

}

