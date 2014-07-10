#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "aes.h"

static void
test_aes_encrypt(long blocks)
{
	long i;
	int fd;
	char key[32], openssl[16], my_aes[16];

	fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC);
	if (fd < 0)
	{
		return;
	}

	for (i = 0; i < blocks; i++)
	{
	}

	close(fd);
}

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

	if (blocks > 0)
	{
		test_aes_encrypt(blocks);
	}

	return 0;
}
