/* spoondr */
/* releaseall.c - releaseall */
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <q.h>


int releaseall (nargs, args)
	int nargs;
{
	STATWORD 	ps;
	unsigned long	*a;
	struct  lentry  *lptr;
	int lid;
	int i, j=0, flag=0, lock_in_use=0;
	a = (unsigned long *)(&args) + (nargs-1);
    disable(ps);
    for ( ; nargs > 0 ; nargs--)
	{
		lid=*a--;
        lptr= &locktab[lid];
        proctab[currpid].locks[lid]=0;
        lptr->lproc[currpid] = 0;

		/* Return error if the lock has been never acquired */
		if (isbadlock(lid) || (lptr= &locktab[lid])->ltype== -1)
        {
            restore(ps);
            return(SYSERR);
			continue;
        }

        /* Return error if the current process does not hold the lock */
		if ( proctab[currpid].locks[lid] != 1 )
		{
		    	restore(ps);
			return(SYSERR);
			continue;
		}

        /* To check if any process is using the lock */
		for (i=0; i <NPROC; i++)
		{
			if (lptr->lproc[i] ==1)
				lock_in_use = 1;
		}
	/* Reset the priority of the processes if the lock is being used */
		if (lock_in_use)
		{
			resetpriority();
			lptr->lprio = maxWaitQueue();
			resched();
			continue;
		}
	/* Traverse the queue to update the type */	
		
		if (nonempty(lptr->lqhead))
        	{
			do
			{
				flag =0;
				if ((q[q[(lptr->lqtail)].qprev].qtype) == READ)
				{
					lptr->ltype = READ;
					flag =1;
				}
				else
					lptr->ltype = WRITE;

                proctab[q[(lptr->lqtail)].qprev].lockid = -1;
                ready(getlast(lptr->lqtail), RESCHNO);

			} while (flag && ((q[q[(lptr->lqtail)].qprev].qtype) == READ));
        	}	
		else
		{
			for ( i=0; i <NPROC; i++)
			{
				if ( proctab[i].locks[lid] == 1 )
					j++;
			}
			if ( j < 1 )
				lptr->lstate = LUSED;
		}
		lptr->lprio = maxWaitQueue();
		resetpriority();
	}
	resched();
	return OK;
}

void resetpriority()
{
    int maxlock=-1,i;
    for(i=0;i<NLOCK;i++)
    {
        if(proctab[currpid].locks[i] > 0)
        {
            maxlock = max(maxlock,locktab[i].lprio);
        }
    }

    if(maxlock > 0)
            proctab[currpid].pinh = maxlock;
    else
            proctab[currpid].pinh = -1;

	priority_inheritance(proctab[currpid].lockid, currpid);
}

int maxWaitQueue(int lid)
{
	 struct lentry * lptr;
	 int i, maxprio=-1;
	 lptr = &locktab[lid];
	 i=q[lptr->lqhead].qnext;
	 while(i!=lptr->lqtail)
         {
        	if(proctab[i].pprio > maxprio)
                    maxprio=proctab[i].pprio;
            i = q[i].qnext;
         }
 	 return maxprio;
}


