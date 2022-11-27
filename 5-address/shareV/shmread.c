#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm_com.h"

int main ( void ) {
    int running = 1;
    void *shared_memory = NULL;
    struct shared_use_st *shared_stuff;
    int shmid = shmget ( ( key_t ) 1234, sizeof ( struct shared_use_st ), 0666 | IPC_CREAT );

    if ( shmid == -1 ) {
        fprintf ( stderr, "shmget failed\n" );
        exit ( EXIT_FAILURE );
    }

    shared_memory = shmat ( shmid, NULL, 0 ); /* 映射共享内存 */

    if ( shared_memory == ( void * ) -1 ) {
        fprintf ( stderr, "shmat failed\n" );
        exit ( EXIT_FAILURE );
    }

    printf ( "Memory attached at %p\n", shared_memory );
    /* 让结构体指针指向这块共享内存 */
    shared_stuff = ( struct shared_use_st * ) shared_memory;
    shared_stuff->written_by_you = 0; /* 控制读写顺序 */

    while ( running ) { /* 循环的从共享内存中读数据，直到读到“end” */
        if ( shared_stuff->written_by_you ) {
            printf ( "You wrote: %s", shared_stuff->some_text );
            sleep ( 1 ); /* 读进程睡一秒，同时会导致写进程睡一秒，这样做到读了之后再写 */
            shared_stuff->written_by_you = 0;

            if ( strncmp ( shared_stuff->some_text, "end", 3 ) == 0 ) {
                running = 0; /* 结束循环 */
            }
        }
    }

    if ( shmdt ( shared_memory ) == -1 ) { /* 删除共享内存 */
        fprintf ( stderr, "shmdt failed\n" );
        exit ( EXIT_FAILURE );
    }

    exit ( EXIT_SUCCESS );
}
