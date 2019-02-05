/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
        //add for PA0 task5
        int starttime;
        if(syscall_status == 1)
            starttime = ctr1000;
        //

	short	*nxtdev;

	if (isbadpid(pid))
		return(SYSERR);
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;

        //add for PA0 task5
        if (syscall_status == 1)
        {
            int exectime = ctr1000 - starttime;
            struct syscallrecrd record = {currpid, "sys_freemem", 1, exectime};
            updatesyscallform(record);
        }
        //

	return(OK);
}
