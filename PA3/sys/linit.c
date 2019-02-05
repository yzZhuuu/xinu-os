#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

struct lockentry locktable[NLOCKS];
int nextlock;

void linit(){
    int i,j;
    struct lockentry *lptr;
    nextlock = NLOCKS - 1;

    for (i = 0; i < NLOCKS; i++){
        lptr = &locktable[i];
        lptr->lstate = LFREE;
        lptr->lqtail = 1 + (lptr->lqhead = newqueue());
        lptr->lprio = -1;
        for (j = 0; j < NPROC; j++){
            lptr->procBitmap[j] = 0;
        }
    }
}