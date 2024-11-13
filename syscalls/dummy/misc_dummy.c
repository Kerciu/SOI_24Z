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

int maxNDescendants( int* maxDesc, pid_t* who, int N )
{

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