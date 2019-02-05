#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int lcreate(void){
    STATWORD ps;
    int ldes;

	disable(ps);

    if ((ldes = newlock()) == SYSERR){
        restore(ps);
        return (SYSERR);
    }

    restore(ps);
    return ldes;
}

int newlock(){
    int ldes;
    int i;

    for (i = 0; i < NLOCKS; i++){
        ldes = nextlock--;
        if (nextlock < 0)
            nextlock = NLOCKS -1;
        if (locktable[ldes].lstate == LFREE){
            locktable[ldes].lstate = LUSED;
            locktable[ldes].readernum = 0;
            locktable[ldes].writernum = 0;
            return (ldes);
        }
    }
    return (SYSERR);
}