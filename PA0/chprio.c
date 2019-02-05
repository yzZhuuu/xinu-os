/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
        //add for PA0 task5
        int starttime;
        if(syscall_status == 1)
            starttime = ctr1000;
        //

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);

        //add for PA0 task5
        if (syscall_status == 1)
        {
            int exectime = ctr1000 - starttime;
            struct syscallrecrd record = {currpid, "sys_freemem", 1, exectime};
            updatesyscallform(record);
        }
        //

	return(newprio);
}
