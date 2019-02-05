#include <sched.h>

int schedtype;

void setschedclass (int sched_class){
    schedtype = sched_class;
}

int getschedclass(){
    return schedtype;
}