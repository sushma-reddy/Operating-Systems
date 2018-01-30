/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned nbytes;
{
    STATWORD ps;

	mblock *curr = (mblock *)getmem(sizeof(mblock));
	mblock *prev = (mblock *)getmem(sizeof(mblock));

	disable(ps);

	if (nbytes == 0 || proctab[currpid].vmemlist->num == 0) {
		restore(ps);
		kprintf("\nNo memory left\n");
		return NULL;
	}
	nbytes = (unsigned int) roundmb(nbytes);

	curr = proctab[currpid].vmemlist;
	prev = curr;
	int i = 0;
	while(i < proctab[currpid].vmemlist->num)
	{
		if(curr->vlen == nbytes)
		{
			if(i == 0)
			{
				unsigned long ret = curr->vaddr;
				if(proctab[currpid].vmemlist->num == 1)
				{
					proctab[currpid].vmemlist->vaddr = 0;
					proctab[currpid].vmemlist->num = 0;
					proctab[currpid].vmemlist->vlen = 0;
				}
				else
				{
					proctab[currpid].vmemlist->vaddr = curr->next->vaddr;
					proctab[currpid].vmemlist->num = (curr->num)-1;
					proctab[currpid].vmemlist->vlen = curr->next->vlen;
				}
				return((WORD *)ret);
			}
			else
			{
				prev->next = curr->next;
				proctab[currpid].vmemlist->num--;
				proctab[currpid].vmemlist->vlen -= nbytes;
				return((WORD *)curr->vaddr);
			}

		}
		else if(curr->vlen > nbytes)
		{
			unsigned long ret = curr->vaddr;
			curr->vaddr += nbytes;
			curr->vlen -= nbytes;
			return((WORD *)ret);
		}
		prev = curr;
		curr = curr->next;
		i++;
	}

	restore(ps);
	return NULL;
}


