/* ************************************************************************
> File Name:     kernel/shm.c
> Author:        rebelOverWaist
> Created Time:  2022年11月26日 星期六 12时45分56秒
> Description:   
 ************************************************************************/
#include <linux/kernel.h>
#include <linux/sched.h>
#include <sys/shm.h>
#include <linux/mm.h>
#include <errno.h>

shm shm_list[SHM_SIZE] = {
    {0, 0, 0, 0}
};

int sys_shmget(key_t key, size_t size) {
    int i;
    unsigned long page;

    if(size > PAGE_SIZE) {
        printk("shmget: size %u cannot be greater than the page size %ud. \n", size);
        return -ENOMEM;
    }

    if(key == 0) {
        printk("shmget: key cannot be 0.\n");
        return -EINVAL;
    }
    //if have Created
    for(i = 0; i < SHM_SIZE; ++i) {
       if(shm_list[i].key == key) {
           add_mapping(shm_list[i].page);
           return i;
       }
    }
    //create new page mm.h
    page = get_free_page();
    if(!page) // not free page
        return -ENOMEM;
    printk("shmget get memory's address is 0x%08x\n", page);
    for(i = 0; i < SHM_SIZE; ++i) {
        if(shm_list[i].key == 0) {
            shm_list[i].key = key;
            shm_list[i].size = size;
            shm_list[i].page = page;
            return i;
        }
    }
    //cannot found 
    free_page(page);
    return -1;
}

void *sys_shmat(int shmid, void *shmaddr) {
    unsigned long data_base, brk;
    
    if(shmid < 0 || SHM_SIZE <= shmid
            || shm_list[shmid].page == 0
            || shm_list[shmid].key <= 0)
        return (void*)-EINVAL;
    
    data_base = get_base(current->ldt[2]);
    printk("current's data_base = 0x%08x, new page = 0x%08x\n", data_base, shm_list[shmid].page);
    brk = current->brk + data_base;
    current->brk += PAGE_SIZE;

    if(put_page(shm_list[shmid].page, brk) == 0) 
        return (void*)-ENOMEM;
    
    return (void*)(current->brk - PAGE_SIZE);
}

int sys_shmdt(void *shmaddr) {
    //release space for usr space
    //whether if count == 0 free page or not 
    return -1;
}
