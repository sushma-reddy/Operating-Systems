/* spoondr */
/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

bs_map_t bsm_tab[NBS];

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	int i,j ;
    	for(i = 0; i < NBS; i++)
    	{
      		bsm_tab[i].bs_status = BSM_UNMAPPED;
        	for(j=0; j < NPROC; j++){
      			bsm_tab[i].bs_pid[j] = 0;
		}
      		bsm_tab[i].bs_vpno = -1;
      		bsm_tab[i].bs_npages = 0;
      		bsm_tab[i].bs_sem = -1;
      		bsm_tab[i].bs_id = i;
      		bsm_tab[i].bs_vheap = 0;

    	}

    	return OK;

}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	STATWORD ps;
	disable(ps);
	int i = 0;
	for(i = 0 ; i < NBS; i++)
		if(bsm_tab[i].bs_status == BSM_UNMAPPED)
		{
			*avail = i ;
			restore(ps);
			return i;
		}
	restore(ps);
	return(SYSERR);
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	if (i < 0 || i > BSM_ENDID || bsm_tab[i].bs_vheap == 1)
        	return SYSERR;

    	bsm_tab[i].bs_status = BSM_UNMAPPED;
    	bsm_tab[i].bs_pid[currpid] = 0;
    	bsm_tab[i].bs_vpno = 0;
    	bsm_tab[i].bs_npages = 0;
    	bsm_tab[i].bs_sem = 0;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
/* pid and vaddr are the lookup parameters, backing store id 
 * and page offset are stored in the pointers passed */

SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
        int i;
        int virtpage = VADDR_TO_VPAGE(vaddr) ;

        if (isbadpid(pid))
            return SYSERR ;

        struct pentry *pptr = &proctab[pid] ;   /* process pointer */

        for (i = 0; i < NBS; i++){
            if(pptr->bs_used[i].bs_status != BSM_MAPPED)
                continue;
            if((virtpage >= pptr->bs_used[i].bs_vpno) && (virtpage < pptr->bs_used[i].bs_vpno+pptr->bs_used[i].bs_npages)){
                *store = i;
                *pageth = virtpage - proctab[pid].bs_used[i].bs_vpno;
                return OK;
            }
        }

        return SYSERR;
}



/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	bsm_tab[source].bs_status = BSM_MAPPED ;
    bsm_tab[source].bs_vpno = vpno ;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	int store, pageth, i, frm_id;
	int status = bsm_lookup(pid, VPAGE_TO_VADDR(vpno), &store, &pageth);
	struct pentry *pptr = &proctab[pid];
	bs_map_t *bsm = &(pptr->bs_used[store]);
	
    	for(i = vpno; i < (bsm->bs_vpno+bsm->bs_npages) ; i++){
        	frm_id = i - FRAME0;
			if(frm_tab[frm_id].fr_status == FRM_MAPPED){
				write_bs(i*NBPG,store,pageth);
				free_frm(frm_id);
			}
    	}
	
	bsm->bs_status = BSM_UNMAPPED;
	bsm->bs_pid[pid] = 0;
	bsm->bs_vpno = -1;
	bsm->bs_npages = 0;
	bsm->bs_vheap = 0;
	
	return OK;
}


