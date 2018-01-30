/* spoondr */
/* lcreate.c - lcreate, newlock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int newlock();

SYSCALL lcreate()
{
	STATWORD ps;
	int	lock;

	disable(ps);
	if ( (lock=newlock() )==SYSERR) {
		restore(ps);
		return(SYSERR);
	}
	restore(ps);
	return(lock);
}

LOCAL int newlock()
{
	int	lock;
	int	i;

	for (i=0 ; i<NLOCK ; i++) {
		lock = nextlock--;
		if (nextlock < 0)
			nextlock = NLOCK-1;
		if (locktab[lock].lstate==LFREE) {
			locktab[lock].lstate = LUSED;
			return(lock);
		}
	}
	return(SYSERR);
}


