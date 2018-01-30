/* spoondr */
/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
 	struct pentry *currpptr = &proctab[currpid] ;   /* current process pointer */
	int bs_id = source ;
    	if (npages > 256 || npages < 1 || bs_id < 0 || bs_id > BSM_ENDID){
        	kprintf("Parameter Error \n");
        	return SYSERR;
    	}

    	if (bsm_tab[bs_id].bs_vheap == 1){
        	kprintf("Private Heap \n");
        	return SYSERR;
    	}

    	if (bsm_tab[bs_id].bs_status == BSM_AVAIL || virtpage < 4096)
            return SYSERR;

    	if (bsm_tab[bs_id].bs_status == BSM_MAPPED){
        	currpptr->bs_used[bs_id].bs_status = BSM_MAPPED ;
        	currpptr->bs_used[bs_id].bs_vpno = virtpage ;
    	}

    	bsm_map(currpid,virtpage,bs_id,npages); /* Called only when the bs is first mapped */
    	currpptr->bs_used[bs_id] = bsm_tab[bs_id] ;
    	return OK;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
    int pid = getpid();	
    if(virtpage < 4096){
        kprintf("Parameter error \n");
        return SYSERR;
    }
   
    free_frm(virtpage - FRAME0);
    bsm_unmap(pid,virtpage,0);
    
    return OK;	
}
