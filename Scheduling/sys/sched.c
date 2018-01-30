#include <kernel.h>
#include <sched.h>
int scheduler_class = 0;
void setschedclass(int sched_class){
        scheduler_class = sched_class;
}

int getschedclass(){
        return(scheduler_class);
}

