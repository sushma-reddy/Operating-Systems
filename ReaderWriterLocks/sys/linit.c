/* spoondr */
#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <i386.h>
#include <stdio.h>
#include <q.h>
#include <lock.h>

void linit()
{
	nextlock = NLOCK - 1;
	struct  lentry  *lptr;
	int i;
	for ( i=0; i<NLOCK ; i++) {
	    lptr = &locktab[i];
        lptr->lstate = LFREE;
        lptr->ltype  = -1 ;
        lptr-> lcnt  = 0;
        lptr->lprio = -1;
        lptr->lqhead = newqueue() ;
        lptr->lqtail = 1 + (lptr->lqhead);
        }
}
