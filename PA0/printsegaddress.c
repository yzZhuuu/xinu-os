#include <stdio.h>

//cause the required preceding and after address are 4byte away from current address, so choose int(4byte) instead of char(1byte).
extern int etext, edata, end;

void printsegaddress()
{
     kprintf("\nCurrent: ");
     kprintf("etext[0x%08x] = 0x%08x, ", &etext, etext);
     kprintf("edata[0x%08x] = 0x%08x, ", &edata, edata);
     kprintf("ebss[0x%08x] = 0x%08x\n", &end, end);
     kprintf("Preceding: ");
     kprintf("etext[0x%08x] = 0x%08x, ", &etext - 1, *(&etext - 1));
     kprintf("edata[0x%08x] = 0x%08x, ", &edata - 1, *(&edata - 1));
     kprintf("ebss[0x%08x] = 0x%08x\n", &end - 1, *(&end - 1));
     kprintf("After: ");
     kprintf("etext[0x%08x] = 0x%08x, ", &etext + 1, *(&etext + 1));
     kprintf("edata[0x%08x] = 0x%08x, ", &edata + 1, *(&edata + 1));
     kprintf("ebss[0x%08x] = 0x%08x\n", &end + 1, *(&end + 1));
}
