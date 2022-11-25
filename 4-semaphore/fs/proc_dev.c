/* ************************************************************************
> File Name:     proc_dev.c
> Author:        rebelOverWaist
> Format:        utf-8
> Description:   lab-9 proc  kernel
 ************************************************************************/
#include <errno.h>

#include <linux/sched.h> // struct task_struct* task[NR_TASKS]
#include <linux/kernel.h> // include malloc() and free() size should less than 4KB 1 page
// now kernel's head include sprintf() 
// fprintk() 
#include <linux/mm.h> // get free page calc_mem 
#include <linux/head.h>//mm
#include <asm/segment.h>
typedef int (*pr_ptr)(char* buf, int count, off_t* pos);

#define set_bit(bitnr,addr) ({\
register int __res; \
        __asm__("bt %2, %3;setb %%al":"=a"(__res):"a"(0),"r"(bitnr),"m"(*(addr))); \
        __res; })
//hdinfo -> super 
/*
 * total_blocks: 
 * free_blocks: 
 * used_blocks: 
 * total inodes: 
 *
 * */
int read_hd(char* buf, int count, off_t* pos) {
    struct super_block* sb;
    int used = 0;
    int read = 0;
    int i, j;
    char* proc_buf;
    if(!(proc_buf = (char*)malloc(sizeof(1024)))) {
        printk("no memory to alloc!\n");
        return 0;
    }
    //dev 0x301 789
    sb = get_super(0x301);
    //blocks
    read += sprintf(proc_buf+read, "Total blocks:%d\n",sb->s_nzones);
    i = sb->s_nzones;
    while(--i >= 0) {
       if(set_bit(i & 8191, sb->s_zmap[i>>13]->b_data)) 
           ++used;
    }
    read += sprintf(proc_buf+read, "Used blocks:%d\n", used);
    read += sprintf(proc_buf+read, "Free blocks:%d\n", sb->s_nzones-used);
    // inodes
    read += sprintf(proc_buf+read, "Total inodes:%d\n", sb->s_ninodes);
    used = 0;
    i = sb->s_ninodes+1;
    while(--i >= 0){
        if(set_bit(i&8191, sb->s_imap[i>>13]->b_data))
            ++used;
    }
    read += sprintf(proc_buf+read, "Used inodes:%d\n", used);
    read += sprintf(proc_buf+read, "Free inodes:%d\n", sb->s_ninodes-used);
    //to buffer 
    proc_buf[read] = '\0';
    for(i = *pos, j = 0; i < read && j <  count; ++i, ++j)
        put_fs_byte(proc_buf[i], buf+j);
    free(proc_buf);
    *pos += j;
    return j;
}


// psinfo -> struct task_struct* task[NR_TASKS] 
int read_ps(char* buf, int count, off_t* pos) {
    int read = 0;
    int i = *pos, j = 0;
    struct task_struct ** p;
    char* proc_buf;

    if(!(proc_buf = (char*)malloc(1024))) {
        printk("no memory to alloc!");
        return 0;
    }

    read += sprintf(proc_buf+read,"pid\tstate\tfather\tcounter\tstart_time\n");
    for(p = &FIRST_TASK + 1; p <= &LAST_TASK; ++p) {
        if(*p) {
         read += sprintf(proc_buf+read,"%ld\t%ld\t%ld\t%ld\t%ld\n",
                    (*p)->pid, (*p)->state, (*p)->father,
                    (*p)->counter,(*p)->start_time);        
        }
    }
    proc_buf[read] = '\0';
    //printk("read->%d pos->%d count->%d j->%d\n", read, i, count, j);
    //if read 0 byte
    if(!read)
        return 0;
    for(; i < read && j < count; ++i, ++j) {
        if(!proc_buf[i])
            break;
        put_fs_byte(proc_buf[i], buf+j);
    }
    free(proc_buf);
    *pos += j;
    return j;
}

// meminfo -> mm 
// calc_mem function
int read_mem(char* buf, int count, off_t* pos) {
    int i,j,k,free=0;
    long* pg_tbl;
    char* proc_buf;
    int read = 0;
    if(!(proc_buf = (char*)malloc(1024))) {
        printk("no memory to alloc!\n");
        return 0;
    }
    /*
    for(i = 0; i < PAGING_PAGES; ++i) {
        if(!mem_map[i])
            ++free;
    }
    */
    read += sprintf(proc_buf+read, "total page:%d\n", PAGING_PAGES);
   // read += sprintf(proc_buf+read, "free page: %d\n", free);
   // read += sprintf(proc_buf+read, "used page: %d\n", PAGING_PAGES-free);
    
    for(i = 2; i < 1024; ++i) {
        if(1 & pg_dir[i]) {
            pg_tbl = (long*)(0xfffff000 & pg_dir[i]);
            for(j=k=0; j < 1024; ++j) {
                if(pg_tbl[j] & 1)
                    ++k;
            }
            read += sprintf(proc_buf+read,"Pg-dir[%d] uses %d pages\n", i, k);
        }
    }
    
    for(i = *pos, j = 0; i < read && j < count; ++i, ++j)
        put_fs_byte(proc_buf[i], buf+j);
    *pos += j;
    return j;
}


static pr_ptr pr_table[] = {
    read_ps, /* /proc/psinfo  */
    read_hd, /* /proc/hdinfo  */
    read_mem  /* /proc/mminfo  */
};

#define NR_DEVS ((sizeof (pr_table)) / (sizeof (pr_ptr)))

/*
 * dev = no of dev
 * buf = the pointer of  user space uesd for receive datas
 * count = the size of buf
 * pos = the last pos of file pointer point to 
 * */
int read_proc(int dev, char* buf, int count, off_t* pos) {
   int i = 0; 
    pr_ptr call_addr;
    if(dev >= NR_DEVS) 
        return -ENODEV;
    if(!(call_addr = pr_table[dev]))
        return -ENODEV;
     return call_addr(buf, count, pos);
}
