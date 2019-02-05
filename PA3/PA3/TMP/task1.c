#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <sem.h>

#define DEFAULT_LOCK_PRIO 20

void proc_sem(char *msg, int sem){
    kprintf("  %s start\n", msg);
    kprintf ("  %s: to acquire semaphore\n", msg);
    wait(sem);
    kprintf ("  %s: acquired semaphore, sleep 3s\n", msg);
    sleep (3);
    kprintf ("  %s: to release semaphore. [before release, priority is %d]\n", msg, getprio(currpid));
    signal(sem);
    kprintf("  %s: released semaphore. [after release, priority is %d]\n", msg, getprio(currpid));
    kprintf("  %s finish\n", msg);
}

void proc_lock_writer (char *msg, int lck){
    kprintf("  %s start\n", msg);
    kprintf ("  %s: to acquire lock\n", msg);
    lock (lck, WRITE, DEFAULT_LOCK_PRIO);
    kprintf ("  %s: acquired lock, sleep 3s\n", msg); 
    sleep (3);
    kprintf ("  %s: to release lock. [before release, priority is %d]\n", msg, getprio(currpid));
    releaseall (1, lck);
    kprintf("  %s: released lock. [after release, priority is %d]\n", msg, getprio(currpid));
    kprintf("  %s finish\n", msg);
}

void proc_neutral(char *msg){
    kprintf("  %s start\n", msg);
    kprintf("  %s: sleep 3s\n", msg);
    sleep (3);       
    kprintf("  %s finish\n", msg);
}

void test_sem(){
        int     sem;
        int     pid1;
        int     pid2;
        int     pid3;

        kprintf("\nTest 1: Original XINU semaphore cannot prevent priority inversion\n");
        sem = screate(1);

        pid1 = create(proc_sem, 2000, 20, "proc a", 2, "proc a", sem);
        pid2 = create(proc_neutral, 2000, 30, "proc b", 1, "proc b");
        pid3 = create(proc_sem, 2000, 40, "proc c", 2, "proc c", sem);

        resume(pid1);
        resume(pid2);
        resume(pid3);

        sleep(10);
        kprintf("Test 1 finish\n");
}

void test_lock(){
        int     lck;
        int     pid1;
        int     pid2;
        int     pid3;

        kprintf("\nTest 2: Priority Inheritance can prevent priority inversion\n");
        lck = lcreate();

        pid1 = create(proc_lock_writer, 2000, 20, "proc a", 2, "proc a", lck);
        pid2 = create(proc_neutral, 2000, 30, "proc b", 1, "proc b");
        pid3 = create(proc_lock_writer, 2000, 40, "proc c", 2, "proc c", lck);

        resume(pid1);
        resume(pid2);
        resume(pid3);

        sleep(10);
        kprintf("Test 2 finish\n");
}

int main(){

        test_sem();
        test_lock();

        shutdown();
}