/* spoondr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int lock (int ldes1, int type, int priority)
{
	struct  lentry  *lptr;
	struct	pentry	*pptr;
	int i;

	lptr = &locktab[ldes1] ;
	pptr = &proctab[currpid];

    STATWORD ps;
    if (isbadlock(ldes1) || lptr->lstate==LFREE) {
        return(SYSERR);
    }

    if (lptr->ltype == DELETED){
        return(DELETED);
    }
	disable(ps);

    /* 1. The lock is free, i.e., no process is owning it.
     *In this case the process that requested the lock gets the lock
     *and sets the type of locking as READ or WRITE.
     */

	if (lptr->lstate == LUSED && lptr->ltype == -1)
    {
        lptr->ltype = type;
        pptr ->locks[ldes1] = 1;
        lptr->lproc[currpid] = 1;
        lptr->lprio = max(lptr->lprio,proctab[currpid].pprio);
        restore(ps);
        return (OK);
    }

    /* 2. Lock is already acquired:
     * a. For READ:
     * If the requesting process has specified the lock type as READ
     * and has sufficiently high priority
     * (not less than the highest priority writer process waiting for the lock),
     * it acquires the lock, else not.
     */

	else if (lptr->lstate == LUSED && type == READ && priority > highestprio_writer(ldes1))
	{
        pptr->locks[ldes1] =1;
        lptr->lproc[currpid] = 1;
        lptr->lprio = max(lptr->lprio,proctab[currpid].pprio);
        restore(ps);
        return (OK);
    }

    /* lock not acquired, process is put in wait queue */
    else
    {
        pptr->pstate = PRWAIT;
        pptr->lockid = ldes1;
        lptr->lprio = max(lptr->lprio,proctab[currpid].pprio);

        priority_inheritance(ldes1,currpid);
        insert(currpid,lptr->lqhead,priority);
        q[currpid].qtype = type;

        pptr->pwaitret = OK;
        resched();

        lptr->lproc[currpid] = 1;
        proctab[currpid].locks[ldes1] = 1;

        for(i=0;i<NPROC;i++)
        {
            if(proctab[i].lockid == ldes1)
            {
                priority_inheritance(proctab[i].lockid,i);
            }

        }

        restore(ps);
        return pptr->pwaitret;
    }
}


int  highestprio_writer(int lid)
{
	int prev;
	register struct lentry  *lptr;
	lptr= &locktab[lid];
	if (nonempty(lptr->lqhead))
	{
		prev = q[lptr->lqtail].qprev;
		while (q[prev].qtype !=WRITE)
			prev = q[prev].qprev;
		if (q[prev].qtype == WRITE)
			return q[prev].qkey;
		else
			return 0;
	}
	return 0;
}

void priority_inheritance(int lid, int pid)
{
	int i;

	for(i=0;i<NPROC;i++)
	{
	    if(locktab[lid].lproc[i] == 1)
        {
            if(proctab[pid].pinh > 0)
            {
                if(proctab[i].pinh > 0)
                {
                    if(proctab[pid].pinh > proctab[i].pinh)
                        proctab[i].pinh = proctab[pid].pinh;
                }

                else
                {
                    if(proctab[pid].pinh > proctab[i].pprio)
                        proctab[i].pinh = proctab[pid].pinh;
                }

            }
            else
            {
                if(proctab[i].pinh > 0)
                {
                    if(proctab[pid].pprio > proctab[i].pinh)
                        proctab[i].pinh = proctab[pid].pprio;
                }
                else
                {
                    if(proctab[pid].pprio > proctab[i].pprio)
                        proctab[i].pinh = proctab[pid].pprio;
                }

            }
            priority_inheritance(proctab[i].lockid,i);

        	}

	}
return ;

}

