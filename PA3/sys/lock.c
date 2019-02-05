#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

// the arg piro is wait prio
int lock (int ldes1, int type, int priority){
    STATWORD ps;
    struct lockentry *lptr;

	disable(ps);
    lptr = &locktable[ldes1];

    if (isbadlock(ldes1) || lptr->lstate == LFREE || lptr->lstate == DELETED){
        restore(ps);
        return SYSERR;
    }

    //the lock is free, assign it to the proc. No prio issue happens in this situation
    if (lptr->lstate == LUSED){
        if (type == READ){
            lptr->lstate = READ;
            lptr->readernum++;
        }
        else{
            lptr->lstate = WRITE;
            lptr->writernum++;
        }
        lptr->procBitmap[currpid] = 1;
        proctab[currpid].ltable[ldes1] = 1;

        restore(ps);
        return (OK);
    }

    //the lock is not free, held by reader(s)
    if (lptr->lstate == READ){
        //the applicant is a reader
        if (type == READ){
            //the wait prio of the applicant is sufficient, get the lock
            if (priority >= getMaxWaitWritePrio(ldes1)){
                lptr->lstate = READ;
                lptr->readernum++;
                lptr->procBitmap[currpid] = 1;
                proctab[currpid].ltable[ldes1] = 1;
                restore(ps);
                return (OK);
            }
            //prio not sufficient, wait
            else {
                proctab[currpid].waitlock = ldes1;
                proctab[currpid].pstate = PRWAIT;
                proctab[currpid].plockret = OK;
                insert(currpid, lptr->lqhead, priority);
                updatePriority_lock(ldes1, currpid);
                q[currpid].qtype = type;
                q[currpid].qtime = ctr1000;
                resched();
                proctab[currpid].waitlock = -1; 
                if (proctab[currpid].plockret != DELETED){
                    lptr->lstate = READ;
                    lptr->readernum++;
                    lptr->procBitmap[currpid] = 1;
                    proctab[currpid].ltable[ldes1] = 1;
                }
                restore(ps);
                return proctab[currpid].plockret;
            }
        }
        //the applicant is a writer, wait
        else {
            proctab[currpid].waitlock = ldes1;
            proctab[currpid].pstate = PRWAIT;
            proctab[currpid].plockret = OK;
            insert(currpid, lptr->lqhead, priority);
            updatePriority_lock(ldes1, currpid);
            q[currpid].qtype = type;
            q[currpid].qtime = ctr1000;
            resched();
            proctab[currpid].waitlock = -1; 
            if (proctab[currpid].plockret != DELETED){
                lptr->lstate = WRITE;
                lptr->writernum++;
                lptr->procBitmap[currpid] = 1;
                proctab[currpid].ltable[ldes1] = 1;
            }
            restore(ps);
            return proctab[currpid].plockret;      
        }
    }
    //the lock is not free, held by a writer, wait
    else if (lptr->lstate == WRITE){
        proctab[currpid].waitlock = ldes1;
        proctab[currpid].pstate = PRWAIT;
        proctab[currpid].plockret = OK;
        insert(currpid, lptr->lqhead, priority);
        updatePriority_lock(ldes1, currpid);        
        q[currpid].qtype = type;
        q[currpid].qtime = ctr1000;
        resched();
        proctab[currpid].waitlock = -1; 
        if (proctab[currpid].plockret != DELETED){
            if (type == READ){
                lptr->readernum++;
                lptr->lstate = READ;
            }
            else{
                lptr->writernum++;
                lptr->lstate = WRITE;
            }
            lptr->procBitmap[currpid] = 1;
            proctab[currpid].ltable[ldes1] = 1;
        }
        restore(ps);
        return proctab[currpid].plockret; 
    }

    restore(ps);
    return (OK);
}

//get the max wait prio of writers in wait queue
int getMaxWaitWritePrio(int ldes){
    int prev;
	register struct lentry  *lptr;
	lptr= &locktable[ldes];
	if (nonempty(locktable[ldes].lqhead))
	{
		prev = q[locktable[ldes].lqtail].qprev;
		while (q[prev].qtype !=WRITE)
			prev = q[prev].qprev;
		if (q[prev].qtype == WRITE)
			return q[prev].qkey;	
		else
			return 0;
	}
	return 0;
}

//update priority, not wait priority
void updatePriority_lock(int ldes, int waitpid){
    struct lockentry *lptr = &locktable[ldes];
    int waitprio = max(proctab[waitpid].pprio, proctab[waitpid].pinh);
    int i = 0;
    int holderpid = -1;

    if (lptr->lprio < waitprio){
        //update lock's lprio
        lptr->lprio = waitprio;

        //get the pid who is holding the lock
        for (i = 0; i < NPROC; i++){
            if (lptr->procBitmap[i] == 1){
                holderpid = i;
                break;
            }
        }

        if (holderpid < 0)
            return SYSERR;

        //if the holder's prio < waiting proc's prio, increase holder's prio
        if (max(proctab[holderpid].pprio, proctab[holderpid].pinh) < waitprio){
            proctab[holderpid].pinh = waitprio;

            //the holder's prio has changed, trans the change
            if (proctab[holderpid].waitlock > -1)
                updatePriority_lock(proctab[holderpid].waitlock, holderpid);
        }
    }
}