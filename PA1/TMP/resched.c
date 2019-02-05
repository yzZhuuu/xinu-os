/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <sched.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
int rrlist_expdist[NQENT] = {0};    /* Round Robin list for EXPDISTSCHED, store proc with same chosen priority */
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	//LINUXSCHED
	if (getschedclass() == LINUXSCHED){

		if ((optr = &proctab[currpid])->pstate == PRCURR){
			optr->pstate = PRREADY;
			//insert current proc into ready list
			insert(currpid, rdyhead, optr->pprio);
		}

		//if q is empty or all proc in q used up their quantum, new epoch
		Bool isnewepoch;
		if (isempty(rdyhead)){
			isnewepoch = TRUE;
		}
		else{
			isnewepoch = TRUE;
			int i = q[rdyhead].qnext;
			while (i != rdytail){
				if (proctab[i].counter > 0){
					isnewepoch = FALSE;
					break;
				}
				i = q[i].qnext;
			}
		}

		int n;
		// if new epoch, initialize the quantum and goodness of each proc in proctab
		if (isnewepoch){
			for (n = 0; n < NPROC; n++){
				proctab[n].basepriority = proctab[n].pprio;
				proctab[n].counter = (int)(proctab[n].counter / 2) + proctab[n].basepriority;
				proctab[n].goodness = proctab[n].basepriority + proctab[n].counter;
			}
		}
		
		//if not new epoch
		else{
			optr->counter = preempt;			

			//update goodness, set 0 for proc which used up their quantum
			for (n = 0; n < NPROC; n++){
				if (proctab[n].counter <= 0){
					proctab[n].goodness = 0;
				}
				else{
					proctab[n].goodness = proctab[n].basepriority + proctab[n].counter;
				}
			}
		}

		//pick highest goodness
		int procchosen;
		int i = q[rdyhead].qnext;
		//if q is empty, run nullproc for default time.
		if (isempty(rdyhead)){
			procchosen = NULLPROC;
			proctab[NULLPROC].counter = QUANTUM;
		}
		//if only one proc in q
		else if ((i == q[rdytail].qprev) && (proctab[i].counter > 0)){
			procchosen = i;
		}
		else{
			//find the proc with highest goodness
			procchosen = NULLPROC;
			while (i != q[rdytail].qprev){
				if (proctab[i].counter <= 0){
					i = q[i].qnext;
					continue;
				}
				else{
					procchosen = i;
					if ((proctab[q[i].qnext].counter > 0) && (proctab[q[i].qnext].goodness > proctab[i].goodness)){
						procchosen = q[i].qnext;
					}
				}
				i = q[i].qnext;
			}
		}

		currpid = dequeue(procchosen);
		nptr = &proctab[currpid];
		nptr->pstate = PRCURR;
	#ifdef	RTCLOCK
		preempt = nptr->counter;		/* reset preemption counter	*/
	#endif
	}

    //EXPDISTSCHED
	else if (getschedclass() == EXPDISTSCHED){
		//1. insert current proc into ready list(q)
		//assign optr and change to ready
		if ((optr = &proctab[currpid])->pstate == PRCURR){
			optr->pstate = PRREADY;
			//insert current proc into ready list
			insert(currpid, rdyhead, optr->pprio);
		}

		//2. deal with round robin first
		int n;
		for(n = 0; n < NQENT; n++){
			//if round robin hasn't finish, run next proc in rrlist
			if (rrlist_expdist[n] != 0){
				//if the proc is still ready, run. else delete from rrlist.
				if (proctab[rrlist_expdist[n]].pstate == PRREADY){
					currpid = dequeue(rrlist_expdist[n]);
					rrlist_expdist[n] = 0;
					nptr = &proctab[currpid];
					nptr->pstate = PRCURR;
					#ifdef	RTCLOCK
						preempt = QUANTUM;		
					#endif	
					ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);	
					return OK;
				}
				else{
					rrlist_expdist[n] = 0;
				}
			}
		}

		//3. get the wanted procid according to randprio
		int randprio;
		int chosenprio;
		int procchosen;
		randprio = (int)expdev(0.1);
		procchosen = q[rdyhead].qnext;
		//if ready list is empty, run null proc to make sure there is at least a proc running
		if (isempty(rdyhead))
			procchosen = NULLPROC;
		else{
			if (randprio < firstkey(rdyhead)){
				procchosen = q[rdyhead].qnext;
			}
			else if (randprio >= lastkey(rdytail)){
				procchosen = q[rdytail].qprev;
			}
			else{
				while (q[procchosen].qkey <= randprio){
					procchosen = q[procchosen].qnext;
				}
				//add the proc with same prio decided by expdev into round robin array, no need to add procchosen, gonna run.
				int p = procchosen;
				int i = 0;
				while (q[p].qnext != rdytail){
					if (q[q[p].qnext].qkey == q[p].qkey){
						rrlist_expdist[i] = q[p].qnext;
						i++;
						p = q[p].qnext;
					}
					else
						break;
				}
			}
		}

		//4. dequeue the wanted proc from ready list and make it PRCURR
		if(procchosen == NULLPROC)
			currpid = NULLPROC;
		else
			currpid = dequeue(procchosen);
		nptr = &proctab[currpid];
		nptr->pstate = PRCURR;
	#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
	#endif

	}
	else{
		//ORIGINAL schedule method
		/* no switch needed if current process priority higher than next*/

		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
		(lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}
		
		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
	#endif

	}
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}
