#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "aes.h"

#define NANOSECONDS  1000000000
#define BENCH(b, f, c)                                            \
	{                                                         \
		struct timespec s, e;                             \
		if (b == 1) {                                     \
			clock_gettime(CLOCK_REALTIME, &s);        \
		}                                                 \
		f;                                                \
		if (b == 1) {                                     \
			clock_gettime(CLOCK_REALTIME, &e);        \
			c.tv_sec += (e.tv_sec - s.tv_sec);        \
			if (e.tv_nsec >= s.tv_nsec) {             \
				c.tv_nsec += (e.tv_nsec - s.tv_nsec);  \
			}                                         \
			else {                                    \
				c.tv_sec--;                       \
				c.tv_nsec += NANOSECONDS +        \
				    (e.tv_nsec - s.tv_nsec);      \
			}                                         \
			if (c.tv_nsec >= NANOSECONDS) {           \
				c.tv_nsec -= NANOSECONDS;         \
				c.tv_sec++;                       \
			}                                         \
		}                                                 \
	}


static void
test_aes_encrypt(long blocks, int bench)
{
	long i;
	int fd;
	uint8_t key[32], openssl[16], my_aes[16];
	AES_KEY aes_key;
	struct timespec openssl_clock, my_aes_clock;

	fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC);
	if (fd < 0) {
		return;
	}

	memset(key, 0, sizeof(key));
	memset(openssl, 0, sizeof(key));
	memset(my_aes, 0, sizeof(key));
	memset(&openssl_clock, 0, sizeof(openssl_clock));
	memset(&my_aes_clock, 0, sizeof(my_aes_clock));

	for (i = 0; i < blocks; i++) {
		ssize_t n;

		if ((n = read(fd, key, sizeof(key))) < 0) {
			goto err;
		}
		if ((n = read(fd, openssl, sizeof(openssl))) < 0) {
			goto err;
		}

		memcpy(my_aes, openssl, sizeof(my_aes));

		AES_set_encrypt_key(key, sizeof(key)*8, &aes_key);

		BENCH(bench, AES_encrypt(openssl, openssl, &aes_key), openssl_clock);
		BENCH(bench, aes_encrypt_block(my_aes, my_aes, &aes_key), my_aes_clock);

		if (memcmp(openssl, my_aes, sizeof(openssl)) == 0) {
			printf(".");
		}
		else {
			printf("x");
		}
	}

	printf("\n");

	if (bench == 1) {
		printf("openssl: %ld.%ld\n", openssl_clock.tv_sec, openssl_clock.tv_nsec);
		printf("my aes: %ld.%ld\n", my_aes_clock.tv_sec, my_aes_clock.tv_nsec);
	}

err:
	close(fd);
}

int
main(int argc, char **argv)
{
	long blocks;
	char *nptr;
	char ch;
	int bench;

	bench = 0;
	blocks = 10;
	while ((ch = getopt(argc, argv, "b")) != -1) {
		switch (ch) {
		case 'b':
			bench = 1;
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc >= 1) {
		long b = strtol(argv[0], &nptr, 10);
		if (argv[1] != nptr) {
			blocks = b;
		}
	}

	if (blocks > 0) {
		test_aes_encrypt(blocks, bench);
	}

	return 0;
}
