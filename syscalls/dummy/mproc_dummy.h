#ifndef MPROC_H
#define MPROC_H

/*
    DUMMY FILE FOR MPROC TABLE, DO NOT COMPILE
*/

#include <sys/types.h>
#include <signal.h>

#define MPROC_TABLE
#define NR_SEGS 0
#define NR_PROCS 0

#define MPROC_TABLE
#define NR_SEGS 3
#define NR_PROCS 64
#define _NSIG 64

typedef unsigned long vir_bytes;
typedef int pid_t;
typedef int uid_t;
typedef int gid_t;
typedef int dev_t;
typedef unsigned int ino_t;
typedef unsigned int time_t;
typedef void (*sighandler_t)(int);
typedef struct { int dummy; } message;
typedef struct { int dummy; } sigset_t;
typedef struct { int dummy; } mem_map;

struct mem_map {
    vir_bytes mem_vir;
    vir_bytes mem_phys;
    size_t mem_len;
};

struct sigaction {
    sighandler_t sa_handler;
    sigset_t sa_mask;
    int sa_flags;
};

struct mproc {
  struct mem_map mp_seg[NR_SEGS];/* points to text, data, stack */
  char mp_exitstatus;		/* storage for status when process exits */
  char mp_sigstatus;		/* storage for signal # for killed procs */
  pid_t mp_pid;			/* process id */
  pid_t mp_procgrp;		/* pid of process group (used for signals) */
  pid_t mp_wpid;		/* pid this process is waiting for */
  int mp_parent;		/* index of parent process */

  /* Real and effective uids and gids. */
  uid_t mp_realuid;		/* process' real uid */
  uid_t mp_effuid;		/* process' effective uid */
  gid_t mp_realgid;		/* process' real gid */
  gid_t mp_effgid;		/* process' effective gid */

  /* File identification for sharing. */
  ino_t mp_ino;			/* inode number of file */
  dev_t mp_dev;			/* device number of file system */
  time_t mp_ctime;		/* inode changed time */

  /* Signal handling information. */
  sigset_t mp_ignore;		/* 1 means ignore the signal, 0 means don't */
  sigset_t mp_catch;		/* 1 means catch the signal, 0 means don't */
  sigset_t mp_sigmask;		/* signals to be blocked */
  sigset_t mp_sigmask2;		/* saved copy of mp_sigmask */
  sigset_t mp_sigpending;	/* signals being blocked */
  struct sigaction mp_sigact[_NSIG + 1]; /* as in sigaction(2) */
  vir_bytes mp_sigreturn; 	/* address of C library __sigreturn function */

  /* Backwards compatibility for signals. */
  sighandler_t mp_func;		/* all sigs vectored to a single user fcn */

  unsigned mp_flags;		/* flag bits */
  vir_bytes mp_procargs;        /* ptr to proc's initial stack arguments */
  struct mproc *mp_swapq;	/* queue of procs waiting to be swapped in */
  message mp_reply;		/* reply message to be sent to one */
} mproc[NR_PROCS];

/* Flag values */
#define IN_USE          0x001	/* set when 'mproc' slot in use */
#define WAITING         0x002	/* set by WAIT system call */
#define ZOMBIE          0x004	/* set by EXIT, cleared by WAIT */
#define PAUSED          0x008	/* set by PAUSE system call */
#define ALARM_ON        0x010	/* set when SIGALRM timer started */
#define SEPARATE	0x020	/* set if file is separate I & D space */
#define	TRACED		0x040	/* set if process is to be traced */
#define STOPPED		0x080	/* set if process stopped for tracing */
#define SIGSUSPENDED 	0x100	/* set by SIGSUSPEND system call */
#define REPLY	 	0x200	/* set if a reply message is pending */
#define ONSWAP	 	0x400	/* set if data segment is swapped out */
#define SWAPIN	 	0x800	/* set if on the "swap this in" queue */

#define NIL_MPROC ((struct mproc *) 0)


#endif