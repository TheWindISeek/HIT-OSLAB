#ifndef _MM_H
#define _MM_H

#define PAGE_SIZE 4096
/*oslab 9*/
#define LOW_MEM 0x100000
#define PAGING_MEMORY (15*1024*1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)
#define USED 100
/*oslab 9*/

extern unsigned long get_free_page(void);
extern unsigned long put_page(unsigned long page,unsigned long address);
extern void free_page(unsigned long addr);
/*oslab 5*/
extern int add_mapping(unsigned long addr);
/*oslab 5*/
/*oslab 9*/
extern void calc_mem(void);
/*oslab 9*/
#endif
