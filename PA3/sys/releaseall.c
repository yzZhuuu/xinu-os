#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

//Releases the currpid's lock and schedules the next proc(s) to whom the lock should go to. 
int releaseall (nargs, args)
    int nargs;      //numlocks
    int args;       //ldes1, ldes2, ...
{
    unsigned long *a;
    int currldes;
    int retval = OK;
    //int maxprio;
    //int lastqmember;
    int nextproc;
    struct lockentry *lptr;

    STATWORD ps;
    disable(ps);

    a = (unsigned long *)(&args) + (nargs-1); /* last argument	*/
    for ( ; nargs > 0 ; nargs--){
        currldes = *a--;
        lptr = &locktable[currldes];

        if (isbadlock(currldes)){
            retval = SYSERR;
            continue;
        }

        //If the lock doesn't belong to the caller, return SYSERR.
        //if the lock is deleted, return SYSERR
        if (proctab[currpid].ltable[currldes] != 1){
            retval = SYSERR;
            continue;
        }

        disable(ps);
        //set the two bitmap and the reader or writer num
        lptr->procBitmap[currpid] = 0;
        proctab[currpid].ltable[currldes] = 0;
        if (lptr->writernum > 0)
            lptr->writernum--;
        else if (lptr->readernum > 0)
            lptr->readernum--;


        //if the lock is held by other proc(reader), do not pass it
        if (lptr->readernum !=0 || lptr->writernum != 0){
            continue;
        }
        //if no one is holding the lock but the wait list is not empty
        else if (nonempty(lptr->lqhead)){
            nextproc = chooseNextWaitProc(currldes);
            lptr->procBitmap[nextproc] = 1;
            proctab[nextproc].ltable[currldes] = 1;
            q[nextproc].qtype == READ ? lptr->readernum++ : lptr->writernum++;
            dequeue(nextproc);
            ready(nextproc, RESCHNO);       
        }
        //no one is holding the lock and the wait list is empty, set the lock free
        else{
            lptr->lstate = LFREE;  
        }
    }
    //update prio after releaseall the locks
    updatePriority_release(currpid);
    resched();
    restore(ps);
    return retval;
}

void updatePriority_release(int pid){
    int i = 0;
    int maxprio = 0;
    int lockcount = 0;
    int priobeforeupdate = max(proctab[pid].pinh, proctab[pid].pprio);
    int waitldes = proctab[pid].waitlock;
    for (; i < NLOCKS; i++){
        if (proctab[pid].ltable[i] == 1){
            maxprio = max(maxprio, locktable[i].lprio);
            lockcount++;
        }
    }
    proctab[pid].pinh = maxprio;
    if (waitldes > -1 && locktable[waitldes].lprio == priobeforeupdate){
        //if waitlock's lprio equals the original pid's prio, update lprio and the holder's prio and recursive
        decreasePrio(waitldes);
    }

    //if the proc released all the lock it held and is not waiting for other lock, clear its pinh
    if (lockcount == 0 && waitldes < 0)
        proctab[pid].pinh = 0;
}

void decreasePrio(int ldes){
    int i =0;
    int holderpid = -1;
    int maxprio = 0;
    int node = q[locktable[ldes].lqhead].qnext;

    while (node != locktable[ldes].lqtail){
        maxprio = max(maxprio, max(proctab[node].pprio, proctab[node].pinh));
        node = q[node].qnext;
    }
    locktable[ldes].lprio = maxprio;

    //get the holder of ldes
    for (i = 0; i < NPROC; i++){
        if (locktable[ldes].procBitmap[i] == 1){
            holderpid = i;
            break;
        }
    }

    int waitldes = proctab[holderpid].waitlock;
    //if the holder is using pinh, update its pinh
    if (proctab[holderpid].pinh > maxprio){
        int originalpinh = proctab[holderpid].pinh;
        proctab[holderpid].pinh = maxprio;
        if (waitldes > -1 && locktable[waitldes].lprio == originalpinh)
            decreasePrio(waitldes);
    }
}

int chooseNextWaitProc(int ldes){
    int lastqmember = q[locktable[ldes].lqtail].qprev;
    int maxprio = q[lastqmember].qkey;
    unsigned long earlistwaitstarttime = ctr1000;
    int maxwaitpid = -1;
    int writepid = -1;
    unsigned long writewaitstarttime = -1;
    //no equal max prio
    if (q[q[lastqmember].qprev].qkey != maxprio){
        return lastqmember;
    }
    //equal max prio
    else{
        int currnode = lastqmember;

        while (q[currnode].qkey == maxprio){
            //TODO: deal with two or more writers, only choose one
            if (q[currnode].qtype == WRITE){
                writepid = currnode;
                writewaitstarttime = q[currnode].qtime;
            }
            //minwaitstarttime = min(minwaitstarttime, q[currnode].qtime);
            if (q[currnode].qtime < earlistwaitstarttime){
                earlistwaitstarttime = q[currnode].qtime;
                maxwaitpid = currnode;
            }
            currnode = q[currnode].qprev;
        }

        if (writepid >= 0){
            if ((writewaitstarttime - earlistwaitstarttime) < 1000 && (earlistwaitstarttime - writewaitstarttime) < 1000)
                return writepid;
        }
        return maxwaitpid;
    }
}