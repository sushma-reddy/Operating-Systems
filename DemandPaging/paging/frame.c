/* spoondr */
/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

fr_map_t frm_tab[NFRAMES];
/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm(){
    int i,j;
	for(i=0;i<NFRAMES;i++){
		frm_tab[i].fr_status = FRM_UNMAPPED;
		frm_tab[i].fr_pid = -1;
		frm_tab[i].fr_vpno = -1;
		frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = -1;
		frm_tab[i].fr_dirty = 0;

	}
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
    int fr_id;
	for(fr_id=0;fr_id<NFRAMES;fr_id++){
		if(frm_tab[fr_id].fr_status == FRM_UNMAPPED){
            init_frm() ;
			if(grpolicy() == SC){
				SC_build_list(fr_id);
			}
			if(grpolicy() == AGING){
                		kprintf("To be implemented \n");
				//AGING_build_list(fr_id);
			}
			*avail = fr_id;
		}
	}

	if (grpolicy()== AGING){
	kprintf("Not implemented yet!!");
	return SYSERR;
	/*
        fr_id = evict_frame_AGING();
        free_frm(fr_id);
        AGING_build_list(fr_id);
	*/
	}
	else {
        fr_id = evict_frame_SC();
        free_frm(fr_id);
        SC_build_list(fr_id);
	}
    *avail = fr_id;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
    STATWORD ps;
    disable(ps);
    pt_t *pte;
    pd_t *pde ;
    int k = NFRAMES;
    int pid = getpid();	
    if (frm_tab[i].fr_refcnt <= 0) {
        if (frm_tab[i].fr_type == FR_PAGE) {
            pte = (pt_t *) ((i +FRAME0)*NBPG);
            init_pgt(pte);
            init_frm(i);
        } else if (frm_tab[i].fr_type == FR_TBL) {
            pde = (pd_t*) (proctab[pid].pdbr);
            while (k > 0) {
                if (pde->pd_base == (i + FRAME0)) {
                    init_pgd(pde);
                    pde->pd_pres = 0;
                    break;
                }
                pde++;
                k--;
            }
            init_frm(i);

        }

    } else {
        if (frm_tab[i].fr_type == FR_PAGE) {
            pte = (pt_t *) ((i +FRAME0)*NBPG);
            init_pgt(pte);
        }
    }

    restore(ps);
    return OK;
}





