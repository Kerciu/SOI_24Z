#include <lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
	message m;
	int a, b, i;

	a = atoi(argv[1]);
	b = atoi(argv[2]);

	for(i = 0; i < a; ++i)
	{
		if (fork() == 0)
		{
			m.m1_i1 = getpid();
			m.m1_i2 = 1;

			if(_syscall(MM, SETGROUP, &m) == -1)
			{
				printf("Error\n");
				return 1;
			}
			while(1);
		}
	}

	for (i = 0; i < b; ++i)
	{
		if(fork() == 0)
		{
			m.m1_i1 = getpid();
			m.m1_i2 = 2;

			if(_syscall(MM, SETGROUP, &m) == -1)
			{
				printf("Error\n");
				return 1;
			}
			while(1);
		}
	}
}

