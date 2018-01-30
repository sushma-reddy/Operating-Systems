/*spoondr*/
/* Implementing page replacement policies */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

struct node_SC *head_SC = NULL;
struct node_SC *tail_SC = NULL;

//int page_replace_policy = SC ;
//
/* Creates a circular linked list, appends newly created nodes */
void SC_build_list(int fr_id){
	struct node_SC * new_node = (struct node_SC*)getmem(sizeof(struct node_SC));
	new_node->fr_id = fr_id;

    /*If the list is empty then the only node is the new node */
	if(tail_SC == NULL && head_SC == NULL){
		tail_SC = new_node;
		tail_SC->next = tail_SC;
		head_SC = tail_SC;
	}

	/* If the list is non-empty, then insert new node after the tail node */
	else {
		new_node->next = head_SC;
		tail_SC->next = new_node;
		tail_SC = new_node;
	}

}

/* Check access bit of the frame */
int SC_check_bit(int fr_id){
    /* Get the pid mapped to the frame*/
	int pid = frm_tab[fr_id].fr_pid;

	/* Obtain vaddr from the pid */
	int vaddr = VPAGE_TO_VADDR(frm_tab[fr_id].fr_vpno);

	/* Page directory and page table entries are calculated from the vaddr */
	pd_t *pde = proctab[pid].pdbr + sizeof(pd_t)*(vaddr >> 22);
	pt_t *pte = (pde->pd_base)*NBPG + sizeof(pt_t)*((vaddr & 0x003ff000) >> 12);

	/* Check the access bit of frame */
	if(pte->pt_acc == 1)
		return 1;
	else
		return 0;
}

/* Clear access bit of the frame */
void SC_clear_bit(int fr_id){

	int pid = frm_tab[fr_id].fr_pid;
	int vaddr = VPAGE_TO_VADDR(frm_tab[fr_id].fr_vpno);

	/* Calculate the page directory and page table offsets*/
	int pd_offset = vaddr >> 22;
	int pt_offset = ((vaddr & 0x003ff000) >> 12);

	pd_t *pde = proctab[pid].pdbr + sizeof(pd_t)*pd_offset;
	pt_t *pte = pde->pd_base *NBPG + sizeof(pt_t)*pt_offset;

	/* Reset the access bit of the frame */
	pte->pt_acc = 0;

	return;
}

int evict_frame_SC(){
	struct node_SC *curr = head_SC;
	struct node_SC *prev = NULL;
	int i;

	while(curr != NULL){
		i = curr->fr_id;
		/* Loop through the list to find frames of page type */
		if(frm_tab[i].fr_type != FR_PAGE){
			prev = curr;
			curr = curr->next;
		}

		/* finding the frame to evict */
		else if(frm_tab[i].fr_type == FR_PAGE){
            if(SC_check_bit(curr->fr_id)==1){
                SC_clear_bit(curr->fr_id);
                prev = curr;
                curr = curr->next;
            }
            else if(SC_check_bit(curr->fr_id) == 0){
                prev->next = curr->next;
                return i;
            }
		}
	}
	return SYSERR;
}
