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
    char buffer[BUFSIZ];
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

    while ( running ) { /* 循环地向共享内存中写数据 */
        while ( shared_stuff->written_by_you == 1 ) {
            sleep ( 1 ); /* 等到读进程读完之后再写 */
            printf ( "waiting for client...\n" );
        }

        printf ( "Enter some text: " );
        fgets ( buffer, BUFSIZ, stdin );
        strncpy ( shared_stuff->some_text, buffer, TEXT_SZ );
        shared_stuff->written_by_you = 1;

        if ( strncmp ( buffer, "end", 3 ) == 0 ) {
            running = 0; /* 结束循环 */
        }
    }

    if ( shmdt ( shared_memory ) == -1 ) { /* 删除共享内存 */
        fprintf ( stderr, "shmdt failed\n" );
        exit ( EXIT_FAILURE );
    }

    exit ( EXIT_SUCCESS );
}
