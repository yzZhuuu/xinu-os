#include <stdio.h>
#include <lab0.h>

int syscall_status = 0;
static struct syscallrecrd *syscallform;
static int syscallformsize;

void printsyscallsummary()
{
    int i, j, n;
    int formsize;
    struct syscallrecrd *form;
    kprintf("\nvoid printsyscallsummary()\n");
    formsize = 0;
    //sort the syscallform to unite the same system call invoked by the same process, put the sorted data into form.
    for (i = 0; i < syscallformsize; i++)
    {
        if ((syscallform + i)->sorted == 1)
            continue;
        else
        {
            for (j = i + 1; j < formsize; j++)
            {
                if ((syscallform + j)->sorted == 1)
                    continue;
                else
                {
                    if ((syscallform + i)->pid == (syscallform + j)->pid)
                    {
                        if ((syscallform+i)->syscallname == (syscallform + j)->syscallname)
                        {
                            (syscallform + i)->count += 1;
                            (syscallform + i)->avgtime = ((syscallform + i)->avgtime + (syscallform + j)->avgtime)/(syscallform + i)->count;
                            (syscallform + j)->sorted = 1;
                        }
                    }
                 }
            }
            (form + formsize)->pid = (syscallform + j)->pid;
            (form + formsize)->syscallname = (syscallform + j)->syscallname;
            (form + formsize)->count = (syscallform + j)->count;
            (form + formsize)->avgtime = (syscallform + j)->avgtime;
            formsize++;        
        }
    }
    kprintf("calculating, please wait...\n");
    //sort the form to put the data with same pid next to each other.
    for (i = 0, n = 1; i < formsize; i++)
    {
        for (j = i + 1; j < formsize; j++)
        {
            if ((form + i)->pid == (form + j)->pid)
            {
                //exchange the data of form+i+n and form+j
                int kpid, kcount, kavgtime;
                char* ksyscallname;
                kpid = (form + j)->pid;
                ksyscallname = (form + j)->syscallname;
                kcount = (form + j)->count;
                kavgtime = (form + j)->avgtime;
                (form + j)->pid = (form + i + n)->pid;
                (form + j)->syscallname = (form + i + n)->syscallname;
                (form + j)->count = (form + i + n)->count;
                (form + j)->avgtime = (form + i + n)->avgtime;
                (form + i + n)->pid = kpid;
                (form + i + n)->syscallname = ksyscallname;
                (form + i + n)->count = kcount;
                (form + i + n)->avgtime = kavgtime;
                n++;
            }
        }
        i += n;
    }
    //print the form following the layout of PA0 template.
    if (formsize == 0)
    {
        kprintf("No system call has been invoked.\n");
    }
    else
    {
        for (i = 0; i < formsize; i++)
        {
            if (i == 0)
                kprintf("Process [pid:%d]\n\tSyscall: %s, count: %d, average execution time: %d (ms)\n", form->pid, form->syscallname, form->count, form->avgtime);
            else if ((form + i)->pid == (form + i - 1)->pid)
                kprintf("\tSyscall: %s, count: %d, average execution time: %d (ms)\n", (form + i)->syscallname, (form + i)->count, (form + i)->avgtime);
            else
                kprintf("Process [pid:%d]\n\tSyscall: %s, count: %d, average execution time: %d (ms)\n", (form + i)->pid, (form + i)->syscallname, (form + i)->count, (form + i)->avgtime);        
        }
    }       
    //syscallformsize = 0;
}

void syscallsummary_start()
{
    syscallformsize = 0;
    syscall_status = 1;
}

void syscallsummary_stop()
{
    syscall_status = 0;
}

void updatesyscallform(struct syscallrecrd recrd)
{
    (syscallform + syscallformsize)->pid = recrd.pid;
    (syscallform + syscallformsize)->syscallname = recrd.syscallname;
    (syscallform + syscallformsize)->count = recrd.count;
    (syscallform + syscallformsize)->avgtime = recrd.avgtime;
    (syscallform + syscallformsize)->sorted = 0;
    syscallformsize++;
}
