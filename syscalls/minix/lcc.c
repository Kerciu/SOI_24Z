#include <stdio.h>
#include <stdlib.h>
#include <lib.h>

int main(int argc, char** argv)
{
        int value;
        message m;
        int ret1, ret2;

        if( argc != 2)
                return 0;

        value = atoi(argv[1]);

        m.m1_i1 = value;
        ret1 = _syscall( MM, LONGESTPATH, & m );
        ret2 = _syscall( MM, WHOLONGESTPATH, & m);
        printf( "syscall return: Longest path: %d, PID: %d\n", ret1, ret2 );
        return 0;
}