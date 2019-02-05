#include <stdio.h>

static unsigned long	*esp;
static unsigned long	*ebp;

void printtos()
{
     kprintf("\nvoid printtos()\n");
     unsigned long *top;
     int i;
     asm("movl %ebp, ebp");
     asm("movl %esp, esp");
     //ebp is lower than the address of the next statement, therefore the top address before call is the 2nd stack ahead of ebp
     kprintf("Before[0x%08x]: 0x%08x\n", ebp + 2, *(ebp + 2));
     kprintf("After [0x%08x]: 0x%08x\n", ebp + 1, *(ebp + 1));
     top = esp + 1;
     for (i = 1; i < 5; i++)
     {
         if ((ebp - i) > esp)
         {         
             kprintf("\telement[0x%08x]: 0x%08x\n", top - i, *(top - i));
         }
         else if (i == 1)
         {
             kprintf("\tThere is no item been pushed.\n");
         }
         else if (i == 2)
         {
             kprintf("\tThere is no second item been pushed.\n");
         }
         else if (i == 3)
         {
             kprintf("\tThere is no third item been pushed.\n");
         }
         else
         {
             kprintf("\tThere is no %dth item been pushed.\n", i);
         }
         
     }
}
