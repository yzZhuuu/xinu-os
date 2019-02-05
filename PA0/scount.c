/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
        //add for PA0 task5
        int starttime;
        if(syscall_status == 1)
            starttime = ctr1000;
        //


extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE)
        {
            //add for PA0 task5
            if (syscall_status == 1)
            {
                int exectime = ctr1000 - starttime;
                struct syscallrecrd record = {currpid, "sys_freemem", 1, exectime};
                updatesyscallform(record);
            }
            //
            return(SYSERR);
        }

            //add for PA0 task5
            if (syscall_status == 1)
            {
                int exectime = ctr1000 - starttime;
                struct syscallrecrd record = {currpid, "sys_freemem", 1, exectime};
                updatesyscallform(record);
            }
            //
        
	return(semaph[sem].semcnt);
}
