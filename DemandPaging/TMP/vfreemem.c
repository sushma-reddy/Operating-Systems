/* spoondr */
/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	mblock	*block;
	unsigned size;
{
	STATWORD ps;
	disable (ps) ;

    if(size <=0 || block == NULL)
	{
		restore(ps);
		return(SYSERR);
	}

	size = (unsigned)roundmb(size);

	mblock *current = (mblock *)getmem(sizeof(mblock));
	current = proctab[currpid].vmemlist;
	mblock *prev = (mblock *)getmem(sizeof(mblock));

	prev->vaddr = block;
	prev->vlen = size;
	prev->num = current->num+1;
	current->num = 0;
	prev->next = current;

	proctab[currpid].vmemlist = prev;

    restore(ps);
    return(OK);
}
