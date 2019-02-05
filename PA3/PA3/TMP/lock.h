#ifndef _LOCK_H_
#define _LOCK_H_

#define NLOCKS  50
#define LFREE    0
#define LUSED    1
#define READ    2
#define WRITE   3


struct lockentry{
    int lstate;
    int lprio;  //maximum priority among all the processes waiting in the lock's wait queue
    int procBitmap[NPROC];  //whether a proc currently holding the lock
    int lqhead;    //head of wait queue
    int lqtail;
    int readernum;  //the number of reader that holding the lock
    int writernum;  //the number of writer that holding the lock
};

extern struct lockentry locktable[];
extern int nextlock;
extern unsigned long ctr1000;

extern void linit();
extern int lcreate(void);
extern int ldelete(int);
extern int lock(int, int, int);
extern int releaseall(int, int, ...);
extern void decreasePrio(int);
extern void updatePriority_lock(int, int);

#define	isbadlock(s)	(s<0 || s>=NLOCKS)

#endif