#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
    struct pentry *currpptr = &proctab[currpid] ;   /* current process pointer */

    if (npages > 256 || npages < 1 || bs_id < 0 || bs_id > BSM_ENDID){
        kprintf("Parameter Error \n");
        return SYSERR;
    }

    if (bsm_tab[bs_id].bs_vheap == 1){
        kprintf("Private Heap \n");
        return SYSERR;
    }

    if (bsm_tab[bs_id].bs_status == BSM_AVAIL){
        bsm_tab[bs_id].bs_status = BSM_UNMAPPED ;
        bsm_tab[bs_id].bs_npages = npages ;
        bsm_tab[bs_id].bs_pid[currpid] = 1 ;
	//bsm_tab[bs_id].bs_vheap = 0 ;
	currpptr->bs_used[bs_id] = bsm_tab[bs_id] ;
    }

    else{
        bsm_tab[bs_id].bs_pid[currpid] = 1 ;
        currpptr->bs_used[bs_id].bs_status = BSM_UNMAPPED ;
        currpptr->bs_used[bs_id] = bsm_tab[bs_id] ;
    }
    return npages;

}


