
#ifndef _SEM_H
#define _SEM_H

#include <linux/sched.h>

typedef struct semaphore{
    
    char *name;
    struct task_struct *wait_task;
    struct semaphore *next;
    unsigned int value;
    unsigned int lock;
} sem_t;

#endif
