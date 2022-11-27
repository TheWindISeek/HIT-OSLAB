/*
 * Author: rebelOverWaist
 * */
//implement of semaphore
//kernel/semaphore.h

#include <linux/sem.h>

#include <linux/kernel.h>
//for schedule sleep_on wake_up
#include <linux/sched.h>

#include <asm/segment.h>

#include <unistd.h>

#include <asm/system.h>
/*
* a data structure for sem
* struct array
*/
sem_t *sem_head = NULL;
/*
sem_t sem_list[NR_SEM] = {
    {NULL, 0, NULL},
    {NULL, 0, NULL},
    {NULL, 0, NULL},
    {NULL, 0, NULL},
    {NULL, 0, NULL},
    {NULL, 0, NULL},
    {NULL, 0, NULL},
};
unsigned int nr_sem = 0;
*/
int sem_strcmp(char *p, char *s) {
    for(;*p == *s; ++p, ++s) {
        if(*p == '\0')
            return 0;
    }
    return (*p - *s);
}

char* get_string_from_usr(char *usr) {
    unsigned int length = 1;
    char c;
    char *kernel, *p = usr, *first;
    while((c = get_fs_byte(p))) {
        ++length;
        ++p;
    }
    first = kernel = (char*)malloc(length);
    p = usr;
    while((c = get_fs_byte(p))) {
        *first = c;
        ++p;
        ++first;
    }
    *first = '\0';
    return kernel;
}


sem_t* sys_sem_open(const char *name, unsigned int value) {
    //dummy head
    if(!sem_head) {
        sem_head = (sem_t*)malloc(sizeof(sem_t));
        sem_head->next = NULL;
    }
    //check sem name if is created
    char *pname = get_string_from_usr(name);
    sem_t *sem = sem_head;
    while(sem->next) {
        //if found
        if(sem_strcmp(sem->next->name, pname) == 0) {
            return sem->next;
        }
        sem = sem->next;
    }
    //assign value to sem->next
    sem->next = (sem_t*)malloc(sizeof(sem_t));
    sem = sem->next;
    sem->name = pname;
    sem->value = value;
    sem->wait_task = NULL;
    sem->lock = 1;
    // printk("%s semaphore had created!\n", sem->name);
    return sem;
}

int sys_sem_unlink(const char *name) {
    //empty linked list
    if(!sem_head || !sem_head->next) {
        return -1;
    }
    sem_t *sem = sem_head, *tmp;
    char *pname = get_string_from_usr(name);
   // printk("unlink pname->%s\n", pname);
    while(sem->next) {
        if(sem_strcmp(sem->next->name, pname) == 0) {
            //delete it
            tmp = sem->next;
            sem->next = tmp->next;
            //release space
            free(tmp->name);
            free(tmp);
            free(pname);
            return 0;
        }
        sem = sem->next;
    }
    free(pname);
    return -1;
}

int sys_sem_wait(sem_t *sem) {
    cli();
    while(sem->value <= 0) {
        sleep_on(&sem->wait_task);
    }
    sem->value--;
    sti();
    return 0;
}

int sys_sem_post(sem_t* sem) {
    cli();
    ++sem->value;
    if( sem->value <= 1) {
        wake_up(&sem->wait_task);
    } 
    sti();
    return 0;
}
