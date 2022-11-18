/* ************************************************************************
> File Name:     who.c
> Author:        rebelOverWaist
> Created Time:  2022-11-5 11:10
> Description:   system call-who am i & i am 
 ************************************************************************/
#include <errno.h>
// get the errno and  EINVAL 
#include <linux/kernel.h>
// get printk
#include <asm/segment.h>
// static inline unsigned char get get_fs_byte(const char* addr)
// static inline void put_fs_byte(char val, char *addr)
#include <string.h>
// strlen()
/*
 * copy the name to kernel
 * the length of name string should less the 23
 * then return the copy string of char 
 * if the length of name > 23 
 *  return -1;  errno <= EINVAL; 
 * */
static char system_name [24];
int sys_iam(const char* name) {
/*implement system call 
 * API 
 * store the number to eax call int
 * find the kernel function 
 * finished the function the return value -> eax  return int 
 * return API
 * return eax to application*/
    char fs;
    int size = 0;
    char* first = name;
    while((fs = get_fs_byte(first++)))
        ++size;

    if(size > 23) {
        errno = EINVAL;
        printk("\nerrno -> %d EINVAL:%d size%d\n ", errno, EINVAL, size);
        return -1;
    }
    
    first = name;
    size = 0;
    while((fs = get_fs_byte(first++))) {
        system_name[size++] = fs;
    }
    system_name[size] = '\0';
   // printk("lucifer system's first system call sys_iam\n");
   // printk("this is the name ::%s::\n", system_name);
    return size;
}


/*
 * copy string Created by iam() to user's address 
 * no cross-border access allowed
 * name's size = size
 * return the copy number of chars
 * if size < need space
 *      return -1; errno <= EINVAL;
 * */
int sys_whoami(char* name, unsigned int size) {
    unsigned int i = 0;
    unsigned int length = strlen(system_name);
   // printk("\n\nthis is the whoami fun\nlength: %d\n", length);
    //boundary
    if(size-1 < length) {
        errno = EINVAL;
        return -1;
    }
   // printk("exec copy\n");
    //copy 
    while(system_name[i]) {
        put_fs_byte(system_name[i++], name++);
    }
    //printk("from->%d\n",(int)system_name[i]);
   // printk("finished copy\n");
   // printk("i = %d\n", i);
    //end flag '\0'
    put_fs_byte(system_name[i], name);
    //printk("lucifer system's second system call sys_whoami\n");
    return length;
}


