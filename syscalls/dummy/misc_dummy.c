#include "mproc_dummy.h"

/* Dummy constants */
#define PUBLIC

typedef struct mm_in {
    int m1_i1;
    int m2_i2;
} mm_in;

struct mm_in mm_in;

/*

Zwrócić pid procesu mającego najdłuższą ścieżkę potomków prowadzącą do procesu nie
mającego dzieci, zwrócić długość tej ścieżki. Pominąć proces o podanym w parametrze
identyfikatorze pid

Pomysł na rozwiązanie:
  - stos

*/

void pushToStack( int stack[], int depth[], int* top, int procNr, int procDepth )
{
    stack[*top] = procNr;
    depth[*top] = procDepth;
    (*top)++;
}

void popFromStack( int stack[], int depth[], int* top, int* procNr, int* procDepth )
{
    (*top)--;
    *procNr = stack[*top];
    *procDepth = depth[*top];
}

int isChild( int parentProc, int candidate )
{
    return (mproc[candidate].mp_flags & IN_USE) && mproc[candidate].mp_parent == parentProc;
}

int procHasChildren( int currentProc )
{
    int procNr;
    for (procNr = 0; procNr < NR_PROCS; ++procNr)
    {
        if (isChild(currentProc, procNr))
            return 1;
    }

    return 0;
}

void pushChildrenToStack( int stack[], int depth[], int* top, int currentProc, int procDepth)
{
    int i;
    for (i = 0; i < NR_PROCS; ++i)
    {
        if (isChild(currentProc, i))
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
    int top = 0;

    int procNr;

    int currentProc;
    int currentDepth;

    pushToStack(stack, depth, &top, start_proc, 1);

    while (top > 0)
    {
        --top;

        popFromStack(stack, depth, &top, &currentProc, &currentDepth);

        if (mproc[currentProc].mp_pid == excluded || !(mproc[currentProc].mp_flags & IN_USE))
            continue;

        if (!procHasChildren(currentProc))
        {
            /* update max depth */

            maxDepth = currentDepth;

        } else {

            /* push children to the stack */

            pushChildrenToStack(stack, depth, &top, currentProc, currentDepth);
        }
    }

    return maxDepth;
}

void longestPathToChildless( int* longestPath, pid_t* who, pid_t excluded )
{

    int procNr;
    int currentDepth;

    int maxPath = -1;
    int found = -1;

    for (procNr = 0; procNr < NR_PROCS; ++procNr)
    {
        if (mproc[procNr].mp_flags & IN_USE)
        {
            currentDepth = determinePathToChildless(procNr, excluded);

            if (currentDepth > maxPath)
            {
                maxPath = currentDepth;
                found = mproc[procNr].mp_pid;
            }
        }
    }

    *longestPath = maxPath;
    *who = found;
}

PUBLIC int do_longestPathToChildless()
{
    int longestPath = -1;
    pid_t who = -1;
    pid_t exluded = mm_in.m1_i1;

    longestPathToChildless(&longestPath, &who, exluded);

    return longestPath;
}

PUBLIC int do_whoLongestPathToChildless()
{
    int longestPath = -1;
    pid_t who = -1;
    pid_t exluded = mm_in.m1_i1;

    longestPathToChildless(&longestPath, &who, exluded);

    return who;
}

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
    tworzymy dwie tablice, queue i depth

*/

int countDescendants( int proc_nr, int N )
{
    int descendants = 0;
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
            if ((mproc[i].mp_flags & IN_USE) && proc_nr != i && mproc[i].mp_parent == current_proc)
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

void maxNDescendants( int* max_desc, pid_t* who, int N )
{
    int proc_nr;
    int descendants;

    int max_desc_found = -1;
    pid_t found = -1;

    for (proc_nr = 0; proc_nr < NR_PROCS; ++proc_nr)
    {
        if (mproc[proc_nr].mp_flags & IN_USE)
        {
            descendants = countDescendants(proc_nr, N);

            if (descendants > max_desc_found)
            {
                max_desc_found = descendants;
                found = mproc[proc_nr].mp_pid;
            }
        }
    }

    *max_desc = max_desc_found;
    *who = found;
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