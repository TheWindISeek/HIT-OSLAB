#ifndef _HEAD_H
#define _HEAD_H

typedef struct desc_struct {//定义段描述符的数据结构 该结构仅仅说明每个描述符
	unsigned long a,b;//由8个字节 每个描述符共有256项
} desc_table[256];

extern unsigned long pg_dir[1024];//内存页目录数组 每个目录项4字节 从物理0地址开始
extern desc_table idt,gdt;//ldt和gdt

#define GDT_NUL 0
#define GDT_CODE 1
#define GDT_DATA 2
#define GDT_TMP 3

#define LDT_NUL 0
#define LDT_CODE 1
#define LDT_DATA 2

#endif
