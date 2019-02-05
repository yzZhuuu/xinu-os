/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}

	//add the logic to deal with priority issue
	int originalprio = max(pptr->pprio, pptr->pinh);

	pptr->pprio = newprio;

	if (pptr->pinh > 0){
		pptr->pinh = newprio;
	}
	int waitldes = pptr->waitlock;
	struct lockentry *lptr = &locktable[waitldes];
	//update priority only if the proc is in a wait queue of a lock
	if (waitldes > -1 && (lptr->lprio == originalprio || newprio > lptr->lprio)){
		//traverse the wait queue to update lprio
		int node = q[lptr->lqhead].qnext;
		int maxprio = -1;
		while (node != lptr->lqtail){
			maxprio = max(maxprio, max(proctab[node].pprio, proctab[node].pinh));
			node = q[node].qnext;
		}
		lptr->lprio = maxprio;

		//get the holder's pid
		int holderpid = -1;
		int i = 0;
		for (i = 0; i < NPROC; i++){
			if (lptr->procBitmap[i] == 1){
				holderpid = i;
				break;
			}
		}
		//update the holder's priority
		if (max(proctab[holderpid].pprio, proctab[holderpid].pinh) < maxprio){
			proctab[holderpid].pinh = maxprio;
			if (proctab[holderpid].waitlock > -1){
				//trans the change
				updatePriority_lock(proctab[holderpid].waitlock, holderpid);
			}
		}
	}

	restore(ps);
	return(newprio);
}
