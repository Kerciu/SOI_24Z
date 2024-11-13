#include "mproc_dummy.h"

/* Dummy constants */
#define PUBLIC

typedef struct mm_in {
    int m1_i1;
    int m2_i2;
} mm_in;

struct mm_in mm_in;

/*

Zadanie 7

Zwrócić pid procesu mającego największą liczbę potomków na kolejnych N (N podawane jako
parametr) poziomach (dla N równego 3 będą to: dzieci, wnukowie, prawnukowie), zwrócić
liczbą potomków dla tego procesu.

*/

/* Propozycja rozwiązania:

   - liczymy potomków
   - liczymy potomków potomków
   - liczymy potomków potomków potomków

    używamy do tego kolejki

*/

int countDescendants( int proc_nr, int N )
{
    int descendants;
    int i;

    int queue[NR_PROCS];
    int depth[NR_PROCS];

    int queue_start = 0;          /* front of the queue */
    int queue_end = 0;            /* back of the queue */

    int current_proc;
    int current_depth;

    /* initialize queue */
    queue[queue_end] = proc_nr;
    depth[queue_end] = 1;
    ++queue_end;

    while (queue_start < queue_end)
    {
        current_proc = queue[queue_start];
        current_depth = depth[queue_start];

        /* shift the queue front */
        ++queue_start;

        for (i = 0 ; i < NR_PROCS ; ++i)
        {
            /* if it is actually descendant */
            if ((mproc[i].mp_flags & IN_USE) && proc_nr != i && mproc[i].mp_parent == proc_nr )
            {
                /* increase descendants num */
                ++descendants;

                /* do not exceed N depth*/
                if (current_depth <= N)
                {
                    /*
                        if not exceeded add another child to the queue
                        and increase depth
                    */

                    queue[queue_end] = i;
                    depth[queue_end] = current_depth + 1;
                    queue_end++;

                }
            }
        }
    }

    return descendants;
}

void maxNDescendants( int* maxDesc, pid_t* who, int N )
{
    int proc_nr;
    int descendants;

    *maxDesc = -1;
    *who = -1;

    for (proc_nr = 0; proc_nr < NR_PROCS; ++proc_nr)
    {
        if (mproc[proc_nr].mp_flags & IN_USE)
        {
            descendants = countDescendants(proc_nr, N);

            if (descendants > maxDesc)
            {
                *maxDesc = descendants;
                *who = mproc[proc_nr].mp_pid;
            }
        }
    }
}

PUBLIC int do_maxNDescendants()
{
    int maxDesc = -1;
    pid_t who = -1;
    int N = mm_in.m1_i1;

    maxNDescendants(&maxDesc, &who, N);

    return maxDesc;
}

PUBLIC int do_whoMaxNDescendants()
{
    int maxDesc = -1;
    pid_t who = -1;
    int N = mm_in.m1_i1;

    maxNDescendants(&maxDesc, &who, N);

    return who;
}