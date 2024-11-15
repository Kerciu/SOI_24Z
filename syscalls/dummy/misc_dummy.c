#include "mproc_dummy.h"

/* Dummy constants */
#define PUBLIC

typedef struct mm_in {
    int m1_i1;
    int m1_i2;
} mm_in;

struct mm_in mm_in;

/*

========================================================================

Zadanie 2

Zwrócić pid procesu mającego najdłuższą ścieżkę potomków prowadzącą do procesu nie
mającego dzieci, zwrócić długość tej ścieżki. Pominąć proces o podanym w parametrze
identyfikatorze pid

Pomysł na rozwiązanie:
  - stos

========================================================================

*/

void pushToStack( int stack[], int depth[], int* top, int procNr, int procDepth )
{
    if (*top >= NR_PROCS) return;       /* stack overflow */

    stack[*top] = procNr;
    depth[*top] = procDepth;
    (*top)++;
}

void popFromStack( int stack[], int depth[], int* top, int* procNr, int* procDepth )
{
    if (*top <= 0) return;              /* stack underflow */

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
    int top = 0;

    int procNr;

    int visitedProcs[NR_PROCS] = {0};

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
            /* update max depth */
            if (currentDepth > maxDepth)
            {
                maxDepth = currentDepth;
            }

        } else {

            /* push children to the stack */
            pushChildrenToStack(stack, depth, &top, currentProc, currentDepth, excluded);
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
        if (!(!(mproc[procNr].mp_flags & IN_USE) || mproc[procNr].mp_pid == excluded))
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

========================================================================

Zadanie 4

Zwrócić pid procesu mającego największą liczbę potomków (dzieci, wnuków …) mieszczącą się
w przedziale <A, B> (A i B podawane jako parametry), zwrócić liczbą potomków dla tego
procesu.

Pomysł na rozwiązanie:
- kolejka, liczymy wszystkich możliwych potomków procesu,
  a przy sprawdzeniu w funkcji przypisującej maksa odrzucamy,
  jesli nie jest w przedziale

========================================================================

*/

int countAllDescendants( int proc_nr, int A_bound, int B_bound )
{
    int descendants = 0;
    int i;

    int queue[NR_PROCS];

    int queue_start = 0;
    int queue_end = 0;

    int current_proc;

    /* queue initialization */
    queue[queue_end++] = proc_nr;

    while (queue_start < queue_end)
    {

        current_proc = queue[queue_start++];

        for (i = 0; i < NR_PROCS; ++i)
        {
            if (mproc[i].mp_flags & IN_USE && proc_nr != i && mproc[i].mp_parent == current_proc)
            {
                ++descendants;
                queue[queue_end++] = i;
            }
        }
    }

    return descendants;
}

void maxDescendantsInBounds( int* max_desc, pid_t* who, int A, int B )
{
    int proc_nr;
    int descendants;

    int max_desc_found = -1;
    pid_t found = -1;

    for (proc_nr = 0; proc_nr < NR_PROCS; ++proc_nr)
    {
        if (mproc[proc_nr].mp_flags & IN_USE)
        {
            descendants = countAllDescendants(proc_nr, A, B);

            if (descendants > max_desc_found && A <= descendants <= B)
            {
                max_desc_found = descendants;
                found = mproc[proc_nr].mp_pid;
            }
        }
    }

    *max_desc = max_desc_found;
    *who = found;
}

PUBLIC int do_maxDescendantsInBounds()
{
    int maxDesc = -1;
    pid_t who = -1;
    int A = mm_in.m1_i1;
    int B = mm_in.m1_i2;

    maxDescendantsInBounds(&maxDesc, &who, A, B);

    return maxDesc;
}

PUBLIC int do_whoMaxDescendantsInBounds()
{
    int maxDesc = -1;
    pid_t who = -1;
    int A = mm_in.m1_i1;
    int B = mm_in.m1_i2;

    maxDescendantsInBounds(&maxDesc, &who, A, B);

    return who;
}

/*

========================================================================

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

========================================================================

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
                if (current_depth < N)
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

int countDescendantsRecursive( int proc_nr, int curr_depth, int N )
{
    int descendants = 0;
    int i;

    for (i = 0; i < NR_PROCS; ++i)
    {
        if ((mproc[i].mp_flags & IN_USE) && proc_nr != i && mproc[i].mp_parent == proc_nr)
        {

            if (curr_depth < N)
            {
                descendants = countDescendantsRecursive(i, curr_depth + 1, N);
            }

            ++descendants;
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
            descendants = countDescendantsRecursive(proc_nr, 1, N);

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