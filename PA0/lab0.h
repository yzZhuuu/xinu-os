struct syscallrecrd
{
    int pid;
    char *syscallname;
    int count;
    int avgtime;
    int sorted;    //1 for sorted, 0 for unsorted
};

extern int syscall_status;
extern unsigned long ctr1000;

extern void updatesyscallform(struct syscallrecrd recrd);
