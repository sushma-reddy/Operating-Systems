/* spoondr */
/* ldelete.c - ldelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

SYSCALL ldelete(int lid)
{
	STATWORD ps;
	int	pid,i;
	struct	lentry	*lptr;

	disable(ps);
	if (isbadlock(lid) || locktab[lid].lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	lptr = &locktab[lid];
	lptr->lstate = LFREE;
	for (i =0; i <NPROC; i++)
	{
		lptr->lproc[i]=0;
	}
	for (i =0; i <NLOCK; i++)
	{
		proctab[i].locks[lid] =0;
	}
	if (nonempty(lptr->lqhead)) {
		while( (pid=getfirst(lptr->lqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    proctab[pid].lockid = -1;
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	restore(ps);
	return(OK);
}


