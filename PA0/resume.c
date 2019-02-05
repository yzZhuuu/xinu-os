/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */
SYSCALL resume(int pid)
{
        //add for PA0 task5
        int starttime;
        if(syscall_status == 1)
            starttime = ctr1000;
        //

	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
		return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);

        //add for PA0 task5
        if (syscall_status == 1)
        {
            int exectime = ctr1000 - starttime;
            struct syscallrecrd record = {currpid, "sys_freemem", 1, exectime};
            updatesyscallform(record);
        }
        //

	return(prio);
}
