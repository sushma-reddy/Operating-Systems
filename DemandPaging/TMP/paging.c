/* spoondr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <paging.h>

int global_pt[4];        /* global page tables */
fr_map_t frm_tab[NFRAMES];

int init_pgt(pt_t *pte){
    int i;
    for (i=0; i < NFRAMES ; i++) {
        pte[i].pt_pres = 0;
        pte[i].pt_write = 0;
        pte[i].pt_user = 0;
        pte[i].pt_pwt = 0;
        pte[i].pt_pcd = 0;
        pte[i].pt_acc = 0;
        pte[i].pt_dirty = 0;
        pte[i].pt_mbz = 0;
        pte[i].pt_global = 0;
        pte[i].pt_avail = 0;
        pte[i].pt_base = 0;
    }
    return OK;
}

/* Obtains an available free page frame,
initializes all the 1024 page table entries of that page frame
and returns the frame id */

int get_pgt(int pid)
{
    int i;
    pt_t *pte;
    int fr_id = 0;
    STATWORD ps;
    disable(ps);

    fr_id = get_frm();
    if (fr_id == SYSERR) {
        kprintf("No free frames available!!");
        restore(ps);
        return SYSERR;
    }

    /* Frames are initialized */
    frm_tab[fr_id].fr_status = FRM_MAPPED;
    frm_tab[fr_id].fr_pid = pid;
    frm_tab[fr_id].fr_vpno = -1;
    frm_tab[fr_id].fr_type = FR_TBL;


    pte =(pt_t*) ((fr_id +FRAME0)*NBPG);
    // init_pgt(pte);
    /* Initialize the Page Table entries of page frame*/
    for (i=0; i < NFRAMES ; i++) {
        pte[i].pt_pres = 0;
        pte[i].pt_write = 0;
        pte[i].pt_user = 0;
        pte[i].pt_pwt = 0;
        pte[i].pt_pcd = 0;
        pte[i].pt_acc = 0;
        pte[i].pt_dirty = 0;
        pte[i].pt_mbz = 0;
        pte[i].pt_global = 0;
        pte[i].pt_avail = 0;
        pte[i].pt_base = 0;
    }

    restore(ps);
    return fr_id;
}


/* Four global page tables corresponding to the first 16 MB of the memory are initialized*/

int init_GlobalPT(){
    int i,j;
    pt_t *pte;
    STATWORD ps;
    disable(ps);

    for (j = 0; j < 4; j++) {
        global_pt[j] = get_pgt(NULLPROC);
        pte = (pt_t*)((global_pt[j]+FRAME0)*NBPG) ;
        for (i=0; i < NFRAMES ; i++) {
            pte[i].pt_pres = 1;
            pte[i].pt_write = 1;
            pte[i].pt_base = ((j * 1024) + i);
        }
    }

    restore(ps);
    return OK;
}

int init_pgd(pd_t *pde){
    int i;
    for (i=0; i < NFRAMES ; i++) {
        pde[i].pd_pres = 0;
        pde[i].pd_write = 0;
        pde[i].pd_user = 0;
        pde[i].pd_pwt = 0;
        pde[i].pd_pcd = 0;
        pde[i].pd_acc = 0;
        pde[i].pd_fmb = 0;
        pde[i].pd_mbz = 0;
        pde[i].pd_global = 0;
        pde[i].pd_avail = 0;
        pde[i].pd_base = 0;
    }
    return OK;
}    


int get_pgd(int pid)
{
    int i, fr_id = 0 ;
    pd_t *pde;

    STATWORD ps;
    disable(ps);

    fr_id = get_frm();
    if (fr_id == SYSERR) {
        kprintf("No free frames available!!");
        restore(ps);
        return SYSERR;
    }

    frm_tab[fr_id].fr_status = FRM_MAPPED;
	frm_tab[fr_id].fr_pid = pid;
	frm_tab[fr_id].fr_vpno = -1;
	frm_tab[fr_id].fr_refcnt = 4;
	frm_tab[fr_id].fr_type = FR_DIR;
	frm_tab[fr_id].fr_dirty = 0;

    pde = (pd_t *)((fr_id +FRAME0)*NBPG);
    // init_pgd(pde) ;
    for (i=0; i < NFRAMES ; i++) {
        pde[i].pd_pres = 0;
        pde[i].pd_write = 0;
        pde[i].pd_user = 0;
        pde[i].pd_pwt = 0;
        pde[i].pd_pcd = 0;
        pde[i].pd_acc = 0;
        pde[i].pd_fmb = 0;
        pde[i].pd_mbz = 0;
        pde[i].pd_global = 0;
        pde[i].pd_avail = 0;
        pde[i].pd_base = 0;
    }

    restore(ps);
    return fr_id;
}
