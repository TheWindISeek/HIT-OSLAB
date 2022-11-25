/*
 *  linux/kernel/exit.c
 *
 *  (C) 1991  Linus Torvalds
 */

#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/tty.h>
#include <asm/segment.h>

int sys_pause(void);
int sys_close(int fd);

void release(struct task_struct * p)
{
	int i;

	if (!p)
		return;
	for (i=1 ; i<NR_TASKS ; i++)
		if (task[i]==p) {
			task[i]=NULL;
			free_page((long)p);
			schedule();
			return;
		}
	panic("trying to release non-existent task");
}

static inline int send_sig(long sig,struct task_struct * p,int priv)
{
	if (!p || sig<1 || sig>32)
		return -EINVAL;
	if (priv || (current->euid==p->euid) || suser())
		p->signal |= (1<<(sig-1));
	else
		return -EPERM;
	return 0;
}

static void kill_session(void)
{
	struct task_struct **p = NR_TASKS + task;
	
	while (--p > &FIRST_TASK) {
		if (*p && (*p)->session == current->session)
			(*p)->signal |= 1<<(SIGHUP-1);
	}
}

/*
 * XXX need to check permissions needed to send signals to process
 * groups, etc. etc.  kill() permissions semantics are tricky!
 */
int sys_kill(int pid,int sig)
{
	struct task_struct **p = NR_TASKS + task;
	int err, retval = 0;

	if (!pid) {//all processes in current process group 
		while (--p > &FIRST_TASK) {
			if (*p && (*p)->pgrp == current->pid) 
				if ((err=send_sig(sig,*p,1)))
					retval = err;
		}
	}  else if (pid>0) { // send to pid
		while (--p > &FIRST_TASK) {
			if (*p && (*p)->pid == pid) 
				if ((err=send_sig(sig,*p,0)))
					retval = err;
	 	}
	}  else if (pid == -1) { // send to all processes except first process
		while (--p > &FIRST_TASK) {
			if ((err = send_sig(sig,*p,0)))
				retval = err;
		} 
	} else {// send to process group is -pid
		while (--p > &FIRST_TASK) {
			if (*p && (*p)->pgrp == -pid)
				if ((err = send_sig(sig,*p,0)))
					retval = err;			
		}
	}
	return retval;
}

static void tell_father(int pid)
{
	int i;

	if (pid)
		for (i=0;i<NR_TASKS;i++) {
			if (!task[i])
				continue;
			if (task[i]->pid != pid)
				continue;
			task[i]->signal |= (1<<(SIGCHLD-1));
			return;
		}
/* if we don't find any fathers, we just release ourselves */
/* This is not really OK. Must change it to make father 1 */
	printk("BAD BAD - no father found\n\r");
	release(current);
}

int do_exit(long code)
{
	int i;
	free_page_tables(get_base(current->ldt[1]),get_limit(0x0f));
	free_page_tables(get_base(current->ldt[2]),get_limit(0x17));
	for (i=0 ; i<NR_TASKS ; i++)
		if (task[i] && task[i]->father == current->pid) {
			task[i]->father = 1;
			if (task[i]->state == TASK_ZOMBIE)
				/* assumption task[1] is always init */
				(void) send_sig(SIGCHLD, task[1], 1);
		}
	for (i=0 ; i<NR_OPEN ; i++)
		if (current->filp[i])
			sys_close(i);
	iput(current->pwd);
	current->pwd=NULL;
	iput(current->root);
	current->root=NULL;
	iput(current->executable);
	current->executable=NULL;
	if (current->leader && current->tty >= 0)
		tty_table[current->tty].pgrp = 0;
	if (last_task_used_math == current)
		last_task_used_math = NULL;
	if (current->leader)
		kill_session();
	current->state = TASK_ZOMBIE;
	current->exit_code = code;
	/*process exit*/
	fprintk(3, "%d\tE\t%ld\n", current->pid, jiffies);
	/*end print*/
	tell_father(current->father);
	schedule();
	return (-1);	/* just to suppress warnings */
}

int sys_exit(int error_code)
{
	return do_exit((error_code&0xff)<<8);
}

int sys_waitpid(pid_t pid,unsigned long * stat_addr, int options)
{
	int flag, code;
	struct task_struct ** p;

	verify_area(stat_addr,4);
repeat:
	flag=0;
	for(p = &LAST_TASK ; p > &FIRST_TASK ; --p) {//from the last task scan all task
		if (!*p || *p == current) //skip empty and current
			continue;
		if ((*p)->father != current->pid) // not son of current process
			continue;
		if (pid>0) {//pid > 0
			if ((*p)->pid != pid) // not the request
				continue;
		} else if (!pid) {//pid = 0
			if ((*p)->pgrp != current->pgrp) // not the process group
				continue;
		} else if (pid != -1) { // pid < -1 
			if ((*p)->pgrp != -pid)//get process group isn't -pid
				continue;
		}
		switch ((*p)->state) {
			case TASK_STOPPED:// stop
				if (!(options & WUNTRACED))
					continue;
				put_fs_long(0x7f,stat_addr);//置状态信息为0x7f
				return (*p)->pid;//退出 返回子进程的进程号
			case TASK_ZOMBIE://zombie 
				current->cutime += (*p)->utime;//更新当前子进程用户态时间
				current->cstime += (*p)->stime;//系统态时间
				flag = (*p)->pid;
				code = (*p)->exit_code;//取子进程的退出码
				release(*p);//释放僵尸进程
				put_fs_long(code,stat_addr);//置状态信息为退出码
				return flag;//返回子进程pid
			default:
				flag=1;
				continue;
		}
	}
	if (flag) {
		if (options & WNOHANG)//如果options = WNOHANG 立即返回
			return 0;
		current->state=TASK_INTERRUPTIBLE;//当前进程可中断
		/*sleep */
		fprintk(3, "%d\tW\t%ld\n", current->pid, jiffies);
		/* print sleep*/
		schedule();//调度
		if (!(current->signal &= ~(1<<(SIGCHLD-1)))) // 又开始执行本进程
			goto repeat;//但是还是没有收到出SIGCHLD外的信号 继续等待
		else
			return -EINTR;//退出 返回出错码
	}
	return -ECHILD;
}


