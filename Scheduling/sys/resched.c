/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sched.h>
#include <math.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	
//Xinu Default Scheduling
	if (scheduler_class == 0){
	
		/* no switch needed if current process priority higher than next*/
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   		(lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}	
	
		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}	

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;			/* mark it currently running	*/
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
	
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
		/* The OLD process returns here when resumed. */
		return OK;
	}		


//Exponential Scheduling
	if(scheduler_class==EXPDISTSCHED){
        	int randprio = expdev (0.1) ;
        	int nextpid ;
        
        	/*Least priority process scheduled when generated priority < least priority */
        	nextpid = q[rdyhead].qnext;  
        
        	/*Finding the pid to be scheduled according to random priority generated */
        	while(q[nextpid].qkey <= randprio){
	        	nextpid = q[nextpid].qnext;
	        	/*Process with largest priority is chosen when generated priority > highest priority*/
	        	if (nextpid == rdytail){     
		    		nextpid = q[rdytail].qprev;
		    		break;
 	        	}
        	}
        
        	/*Next process id if invalid then poiting to NULL process*/
        	if (nextpid >= NPROC) 
            	nextpid = NULLPROC;
        
        	/*No context switch needed when current running process itself needs to be continued*/
        	if (((optr = &proctab[currpid])->pstate == PRCURR) && ((nextpid == NULLPROC) || ((q[nextpid].qkey > optr->pprio) && (optr->pprio > randprio)))) { 

            	#ifdef RTCLOCK
                	preempt = QUANTUM; 
            	#endif 
        	return (OK);
        	}	

        	/*force context switch*/
        
        	/*Current process state changed and put in ready queue*/
        	if (optr->pstate == PRCURR) { 
            		optr->pstate = PRREADY; 
            		insert(currpid, rdyhead, optr->pprio);  
        	}	
        
        	/*New pointer points to the process to be scheduled, process state changed*/
        	nptr = &proctab[(currpid = dequeue(nextpid))];
	    	nptr->pstate = PRCURR; 	
        	#ifdef  RTCLOCK 
	        	preempt = QUANTUM; /* reset preemption counter	*/ 
        	#endif

        	ctxsw((int) &optr->pesp, (int) optr->pirmask, (int) &nptr->pesp, (int) nptr->pirmask);

      	  	return (OK);
    	}


//Linux Scheduling
	if(scheduler_class==LINUXSCHED){
                int i, nextpid;
		int epoch = 0;
                optr = &proctab[currpid];

                for(i=1;i<NPROC; i++){		
                    if(proctab[i].pstate == PRREADY){ 
			epoch = epoch + proctab[i].linux_quantum;
		    }	
                }
                epoch = epoch + preempt ;
	
                /* continuing current epoch */
                if(epoch!=0){	
                    if (optr->pstate == PRCURR){
                        if(preempt==0){
                            optr->goodness = 0;
			}
                        else{    
                            optr->goodness = optr->goodness - (optr->linux_quantum-preempt);
                        }
                    }
		proctab[currpid].linux_quantum = preempt;	
                }
                
                /* new epoch */
                else{
		    proctab[currpid].linux_quantum = preempt;	
                    for(i=1;i<NPROC; i++){
                        /* For processes that have been never executed */
                        if(proctab[i].goodness == proctab[i].pprio){
                            proctab[i].linux_quantum = proctab[i].pprio;
                        }
                        else{     
                            proctab[i].goodness = proctab[i].pprio + proctab[i].linux_quantum;
                            proctab[i].linux_quantum = proctab[i].pprio + (proctab[i].linux_quantum/2);
                        }
                    }
                }
                
		/*Finding next process id to be scheduled*/
		int highest_good = -1 ;
    		nextpid = q[rdyhead].qnext;
		
    		while(nextpid < NPROC){
        		if(proctab[nextpid].goodness > highest_good)
            			highest_good = proctab[nextpid].goodness;
        		nextpid = q[nextpid].qnext;
    		}	
		nextpid = q[nextpid].qprev; 
		
		/*no context switch needed*/
                if(nextpid == currpid){
                    preempt = proctab[nextpid].linux_quantum;
                    return OK;
                }
		
		if (nextpid > NPROC)
			nextpid = q[rdytail].qprev ;
		
                if (optr->pstate == PRCURR){
        		optr->pstate = PRREADY;
        		insert(currpid,rdyhead,optr->pprio);
    		}		

		nptr = &proctab[(currpid=dequeue(nextpid))];
                nptr->pstate = PRCURR;
                #ifdef  RTCLOCK
                    preempt = nptr->linux_quantum;      
                #endif
                ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                return OK;

 	}

}
