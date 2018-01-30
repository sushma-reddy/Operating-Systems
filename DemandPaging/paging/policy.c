/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

extern int page_replace_policy;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy
 *-------------------------------------------------------------------------
 */

SYSCALL srpolicy(int policy)
{
  STATWORD ps;
    disable(ps);

    if (policy == SC)
        page_replace_policy = SC;
    else if (policy == AGING) {
        kprintf("To be implemented \n");
        return SYSERR;
    }
    else
        return SYSERR;
    restore(ps);
    return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}




