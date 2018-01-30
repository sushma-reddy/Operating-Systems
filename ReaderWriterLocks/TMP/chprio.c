/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;
	struct	pentry	*pptr;
	int i,j ;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;

	if (pptr->lockid >0)
		priority_inheritance(pptr->lockid, pid);

	for(i=0;i<NLOCK;i++)
	{

		if(locktab[i].lproc[pid] > 0)
		{

			for(j=0;j<NPROC;j++)
			{
				if(proctab[j].lockid==i)
				{
					priority_inheritance(i,j);
				}
			}
		}
	}
	if(pptr -> pstate == PRREADY) {
		dequeue(pid);
		insert(pid, rdyhead, selectpriority(pid));
	}
	restore(ps);
	return(newprio);
}
