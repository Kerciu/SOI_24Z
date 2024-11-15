#include <stdio.h>
#include <stdlib.h>
#include <lib.h>

#define PARSING_ERROR -1

void print_help() {
    printf("Program usage:\n");
    printf("  -p <value> : Specify the PID to exclude.\n");
    printf("  -c <value> : Specify the children in the test case.\n");
    printf("  Example: ./program -p 1 -c 10\n");
    return 0;
}

int parse_request(int argc, char** argv, int* pid, int* children)
{
    int i;

    if (argc != 5) {
        print_help();
        return PARSING_ERROR;
    }

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                *pid = atoi(argv[i + 1]);
                i++;
            } else {
                fprintf(stderr, "Error: Missing value for -p\n");
                printf("Type in ./%s -h for help\n", argv[0]);
                return PARSING_ERROR;
            }
        } else if (strcmp(argv[i], "-c") == 0) {
            if (i + 1 < argc) {
                *children = atoi(argv[i + 1]);
                i++;
            } else {
                fprintf(stderr, "Error: Missing value for -c\n");
                printf("Type in ./%s -h for help\n", argv[0]);
                return PARSING_ERROR;
            }
        } else if (strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;

        } else {
            fprintf(stderr, "Error: Unknown argument %s\n", argv[i]);
            printf("Type in ./%s -h for help\n", argv[0]);
            return PARSING_ERROR;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    int value;
    message m;
    int ret1, ret2;
    int pid;
    int children;

    if( parse_request(argc, argv, &pid, &children) != 0 )
    {
        return PARSING_ERROR;
    }

    value = atoi(argv[1]);

    m.m1_i1 = value;
    ret1 = _syscall( MM, LONGESTPATH, & m );
    ret2 = _syscall( MM, WHOLONGESTPATH, & m);
    printf( "syscall return: Longest path: %d, PID: %d\n", ret1, ret2 );
    return 0;
}