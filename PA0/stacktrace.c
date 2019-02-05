/* stacktrace.c - stacktrace */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

static unsigned long	*esp;
static unsigned long	*ebp;

#define STKDETAIL

/*------------------------------------------------------------------------
 * stacktrace - print a stack backtrace for a process
 *------------------------------------------------------------------------
 */
SYSCALL stacktrace(int pid)
{
        //add for PA0 task5
        int starttime;
        if(syscall_status == 1)
            starttime = ctr1000;
        //

	struct pentry	*proc = &proctab[pid];
	unsigned long	*sp, *fp;

	if (pid != 0 && isbadpid(pid))
        {
            //add for PA0 task5
            if (syscall_status == 1)
            {
                int exectime = ctr1000 - starttime;
                struct syscallrecrd record = {currpid, "sys_freemem", 1, exectime};
                updatesyscallform(record);
            }
        //

		return SYSERR;
         }
	if (pid == currpid) {
		asm("movl %esp,esp");
		asm("movl %ebp,ebp");
		sp = esp;
		fp = ebp;
	} else {
		sp = (unsigned long *)proc->pesp;
		fp = sp + 2; 		/* where ctxsw leaves it */
	}
	kprintf("sp %X fp %X proc->pbase %X\n", sp, fp, proc->pbase);
#ifdef STKDETAIL
	while (sp < (unsigned long *)proc->pbase) {
		for (; sp < fp; sp++)
			kprintf("DATA (%08X) %08X (%d)\n", sp, *sp, *sp);
		if (*sp == MAGIC)
			break;
		kprintf("\nFP   (%08X) %08X (%d)\n", sp, *sp, *sp);
		fp = (unsigned long *) *sp++;
		if (fp <= sp) {
			kprintf("bad stack, fp (%08X) <= sp (%08X)\n", fp, sp);
               
                //add for PA0 task5
                if (syscall_status == 1)
                {
                    int exectime = ctr1000 - starttime;
                    struct syscallrecrd record = {currpid, "sys_freemem", 1, exectime};
                    updatesyscallform(record);
                }
                //

			return SYSERR;
		}
		kprintf("RET  0x%X\n", *sp);
		sp++;
	}
	kprintf("MAGIC (should be %X): %X\n", MAGIC, *sp);
	if (sp != (unsigned long *)proc->pbase) {
		kprintf("unexpected short stack\n");
        //add for PA0 task5
        if (syscall_status == 1)
        {
            int exectime = ctr1000 - starttime;
            struct syscallrecrd record = {currpid, "sys_freemem", 1, exectime};
            updatesyscallform(record);
        }
        //

		return SYSERR;
	}
#endif
        //add for PA0 task5
        if (syscall_status == 1)
        {
            int exectime = ctr1000 - starttime;
            struct syscallrecrd record = {currpid, "sys_freemem", 1, exectime};
            updatesyscallform(record);
        }
        //

	return OK;
}
