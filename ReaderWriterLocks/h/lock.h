/* spoondr */
/* sem.h - isbadsem */

#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef	NLOCK
#define	NLOCK		50	    /* number of semaphores */
#endif

#define	LFREE 		'\01'		/* lock is free		    */
#define	LUSED 		'\02'		/* lock is being used   */

#define	READ 		1
#define	WRITE		2

/* Lock table entries */
struct	lentry	{
    int  lkid;           /* lock id                  */
	char lstate;         /* LFREE or LUSED           */
	int  ltype;          /* DELETED or READ or WRITE */
	int	 lcnt;           /* count for this lock      */
	int  lprio;          /* priority of the lock     */
	int  lqhead;         /* q index of head of list  */
	int  lqtail;
	int  lproc[NPROC];  /* bit mask of the process ids */
};


extern	struct	lentry	locktab[];    /* Lock tables */
extern	int	nextlock;

void linit();
int lcreate();
int ldelete(int lockdescriptor);
int lock(int ldes1, int type, int priority);
int releaseall(int numlocks, int ldes1, ...);
int bestinq(int lock);
int release(int pid, int ldes);
extern void deque(int pid, int ldes);
extern int 	highest_write_prio(int ldes);

#define	isbadlock(l)	(l<0 || l>=NLOCK)

#endif
