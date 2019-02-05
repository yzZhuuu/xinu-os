#include <stdio.h>
#include <kernel.h>
#include <proc.h>

static unsigned long	*esp;

void printprocstks(int priority)
{
     int i;
     int count;
     struct pentry *proc;
     kprintf("\nvoid printprocstks(int priority)\n");
     for (i = 0; i < NPROC; i++)
     {
         count = 0;
         if (i != 0 && isbadpid(i))
             return SYSERR;
         else
         {
             proc = &proctab[i];
             if (proc->pprio > priority)
             {
                 kprintf("Process [%s]\n", proc->pname);
                 kprintf("\tpid: %d\n", i);
                 kprintf("\tpriority: %d\n", proc->pprio);
                 kprintf("\tbase: 0x%08x\n", proc->pbase);
                 kprintf("\tlimit: 0x%08x\n", proc->plimit);
                 kprintf("\tlen: %d\n", proc->pstklen);
                 if (i == currpid)
                 {
                     asm("movl %esp, esp");
                     kprintf("\tpointer: 0x %08x\n", esp);
                 }
                 else
                     kprintf("\tpointer: 0x %08x\n", proc->pesp);
                 count++;
             }
         }
     }
     if (count == 0)
     {
         kprintf("There is no such process whos priority is larger than %d.\n", priority);
     }
}
