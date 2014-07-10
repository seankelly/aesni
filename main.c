#include <stdio.h>
#include <stdlib.h>

#include "aes.h"

int
main(int argc, char **argv)
{
	long blocks;
	char *nptr;

	aes_init();

	blocks = 10;
	if (argc > 1)
	{
		long b = strtol(argv[1], &nptr, 10);
		if (argv[1] != nptr)
		{
			blocks = b;
		}
	}

	return 0;
}
