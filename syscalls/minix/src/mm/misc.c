/* Miscellaneous system calls.				Author: Kees J. Bot
 *								31 Mar 2000
 *
 * The entry points into this file are:
 *   do_reboot: kill all processes, then reboot system
 *   do_svrctl: memory manager control
 */

#include "mm.h"
#include <minix/callnr.h>
#include <signal.h>
#include <sys/svrctl.h>
#include "mproc.h"
#include "param.h"

void pushToStack( int stack[], int depth[], int* top, int procNr, int procDepth )
{
    if (*top >= NR_PROCS) {
        printf("Error: Stack overflow\n");
        return;
    }

    stack[*top] = procNr;
    depth[*top] = procDepth;
    (*top)++;
}

void popFromStack( int stack[], int depth[], int* top, int* procNr, int* procDepth )
{
    if (*top <= 0 ) {
        printf("Error: Stack underflow\n");
        return;
    }

    (*top)--;
    *procNr = stack[*top];
    *procDepth = depth[*top];
}

int isChild( int parentProc, int candidate )
{
    if ((mproc[candidate].mp_flags & IN_USE) && mproc[candidate].mp_parent == parentProc)
	return 1;

    else return 0;
}

int procHasChildren( int currentProc )
{
    int procNr;
    for (procNr = 0; procNr < NR_PROCS; ++procNr)
    {
        if (isChild(currentProc, procNr) == 1)
            return 1;
    }

    return 0;
}

void pushChildrenToStack( int stack[], int depth[], int* top, int currentProc, int procDepth, pid_t excluded)
{
    int i;
    for (i = 0; i < NR_PROCS; ++i)
    {
        if (isChild(currentProc, i) == 1 && mproc[i].mp_pid != excluded)
        {
            pushToStack(stack, depth, top, i, procDepth + 1);
        }
    }
}

int determinePathToChildless( int start_proc, pid_t excluded )
{
    int maxDepth = -1;

    int stack[NR_PROCS];
    int depth[NR_PROCS];
    int visitedProcs[NR_PROCS] = {0};
    int top = 0;

    int procNr;

    int currentProc;
    int currentDepth;

    pushToStack(stack, depth, &top, start_proc, 1);

    while (top > 0)
    {
        popFromStack(stack, depth, &top, &currentProc, &currentDepth);

        if (mproc[currentProc].mp_pid == excluded || !(mproc[currentProc].mp_flags & IN_USE))
            continue;

	    if (visitedProcs[currentProc]) continue;
	    visitedProcs[currentProc] = 1;

        if (procHasChildren(currentProc) != 1)
        {

            if (currentDepth > maxDepth)
            {
                maxDepth = currentDepth;
            }

        } else {

            /* push children to the stack */

            pushChildrenToStack(stack, depth, &top, currentProc, currentDepth, excluded);
        }
    }

    /* debug */
    printf("Returning max depth %d\n", maxDepth);
    return maxDepth;
}

void longestChildlessChain( int* longestPath, pid_t* who, pid_t excluded )
{

    int procNr;
    int currentDepth;

    int maxPath = -1;
    int found = -1;

    for (procNr = 0; procNr < NR_PROCS; ++procNr)
    {
        if (!(mproc[procNr].mp_flags & IN_USE) || mproc[procNr].mp_pid == excluded)
            continue;

        currentDepth = determinePathToChildless(procNr, excluded);

        if (currentDepth > maxPath)
        {
            maxPath = currentDepth;
            found = mproc[procNr].mp_pid;
        }
    }

    *longestPath = maxPath;
    *who = found;
}

PUBLIC int do_longestChildlessChain()
{
    int longestPath = -1;
    pid_t who = -1;
    pid_t exluded = mm_in.m1_i1;

    longestPathToChildless(&longestPath, &who, exluded);

    return longestPath;
}

PUBLIC int do_whoLongestChildlessChain()
{
    int longestPath = -1;
    pid_t who = -1;
    pid_t exluded = mm_in.m1_i1;

    longestChildlessChain(&longestPath, &who, exluded);

    return who;
}

/*=====================================================================*
 *			    do_reboot				       *
 *=====================================================================*/
PUBLIC int do_reboot()
{
  register struct mproc *rmp = mp;
  char monitor_code[32*sizeof(char *)];

  if (rmp->mp_effuid != SUPER_USER) return(EPERM);

  switch (reboot_flag) {
  case RBT_HALT:
  case RBT_REBOOT:
  case RBT_PANIC:
  case RBT_RESET:
	break;
  case RBT_MONITOR:
	if (reboot_size >= sizeof(monitor_code)) return(EINVAL);
	if (sys_copy(who, D, (phys_bytes) reboot_code,
		MM_PROC_NR, D, (phys_bytes) monitor_code,
		(phys_bytes) (reboot_size+1)) != OK) return(EFAULT);
	if (monitor_code[reboot_size] != 0) return(EINVAL);
	break;
  default:
	return(EINVAL);
  }

  /* Kill all processes except init. */
  check_sig(-1, SIGKILL);

  tell_fs(EXIT, INIT_PROC_NR, 0, 0);	/* cleanup init */
  tell_fs(EXIT, MM_PROC_NR, 0, 0);	/* cleanup for myself */

  tell_fs(SYNC,0,0,0);

  sys_abort(reboot_flag, MM_PROC_NR, monitor_code, reboot_size);
  /* NOTREACHED */
}

/*=====================================================================*
 *			    do_svrctl				       *
 *=====================================================================*/
PUBLIC int do_svrctl()
{
  int req;
  vir_bytes ptr;

  req = svrctl_req;
  ptr = (vir_bytes) svrctl_argp;

  /* Is the request for the kernel? */
  if (((req >> 8) & 0xFF) == 'S') {
	return(sys_sysctl(who, req, mp->mp_effuid == SUPER_USER, ptr));
  }

  switch(req) {
  case MMSIGNON: {
	/* A user process becomes a task.  Simulate an exit by
	 * releasing a waiting parent and disinheriting children.
	 */
	struct mproc *rmp;
	pid_t pidarg;

	if (mp->mp_effuid != SUPER_USER) return(EPERM);

	rmp = &mproc[mp->mp_parent];
	tell_fs(EXIT, who, 0, 0);

	pidarg = rmp->mp_wpid;
	if ((rmp->mp_flags & WAITING) && (pidarg == -1
		|| pidarg == mp->mp_pid || -pidarg == mp->mp_procgrp))
	{
		/* Wake up the parent. */
		rmp->reply_res2 = 0;
		setreply(mp->mp_parent, mp->mp_pid);
		rmp->mp_flags &= ~WAITING;
	}

	/* Disinherit children. */
	for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++) {
		if (rmp->mp_flags & IN_USE && rmp->mp_parent == who) {
			rmp->mp_parent = INIT_PROC_NR;
		}
	}

	/* Become like MM and FS. */
	mp->mp_pid = mp->mp_procgrp = 0;
	mp->mp_parent = 0;
	return(OK); }

  case MMSWAPON: {
	struct mmswapon swapon;

	if (mp->mp_effuid != SUPER_USER) return(EPERM);

	if (sys_copy(who, D, (phys_bytes) ptr,
		MM_PROC_NR, D, (phys_bytes) &swapon,
		(phys_bytes) sizeof(swapon)) != OK) return(EFAULT);

	return(swap_on(swapon.file, swapon.offset, swapon.size)); }

  case MMSWAPOFF: {
	if (mp->mp_effuid != SUPER_USER) return(EPERM);

	return(swap_off()); }

  default:
	return(EINVAL);
  }
}
