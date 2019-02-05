#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int ldelete (int ldes){
    STATWORD ps;
    int pid;
    int i;
    struct lockentry *lptr;
 
	disable(ps);
    if (isbadlock(ldes) || locktable[ldes].lstate == LFREE){
        restore(ps);
		return (SYSERR);
    }

    lptr = &locktable[ldes];
    lptr->lstate = LFREE;
    lptr->lprio = -1;
    lptr->readernum = 0;
    lptr->writernum = 0;
    for (i = 0; i < NPROC; i++){
        lptr->procBitmap[i] = 0;
    }
    if (nonempty(lptr->lqhead)){
        while ((pid=getfirst(lptr->lqhead)) != EMPTY){
            proctab[pid].plockret = DELETED;
            ready(pid, RESCHNO);
        }
        resched();
    }
    restore(ps);
    return(OK);
}