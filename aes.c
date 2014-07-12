#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"

static int use_aesni(void);
static void aes_encrypt_block_openssl(const uint8_t *, uint8_t *, const AES_KEY *);
static void aes_encrypt_block_aesni(const uint8_t *, uint8_t *, const AES_KEY *);

#define CPUID_EAX 1
#define CPUID_ECX_BIT (0x2000000)

static int
use_aesni(void)
{
	/* I only have 64 bit systems on which to test. */
#ifdef __x86_64__
	int a, b, c, d;
	asm volatile (
		"cpuid"
		: "=a" (a),
		"=b" (b),
		"=c" (c),
		"=d" (d)
		: "a" (CPUID_EAX)

	);

#ifdef DEBUG
	if ((c & CPUID_ECX_BIT)) {
		printf("AES-NI enabled\n");
	}
#endif

	return (c & CPUID_ECX_BIT) ? 1 : -1;
#else
	return -1;
#endif
}

void
aes_encrypt_block(const uint8_t *in, uint8_t *out, const AES_KEY *key)
{
	static int aesni;

	if (aesni == 0) {
		aesni = use_aesni();
	}

	if (aesni == 1) {
		aes_encrypt_block_aesni(in, out, key);
	}
	else {
		aes_encrypt_block_openssl(in, out, key);
	}
}

static void
aes_encrypt_block_openssl(const uint8_t *in, uint8_t *out, const AES_KEY *key)
{
	AES_encrypt(in, out, key);
}

static void
aes_encrypt_block_aesni(const uint8_t *in, uint8_t *out, const AES_KEY *key)
{
#if 0
	AES_encrypt(in, out, key);
#else
	int final_index;
	uint8_t aes_state[16];
	uint8_t aes_key[4 * 4 * (AES_MAXNR + 1)];

	memcpy(aes_key, key->rd_key, sizeof(aes_key));

	/*
	 * Load in block into xmm0. Blocks from round keys will be loaded into
	 * xmm1-xmm7, rotating around as necessary. Intermediate state and
	 * final result are in xmm0.
	 * Loading into memory using unaligned loads because can't guarantee at
	 * this moment that any of the input or output variables are aligned on
	 * a 16-byte boundary.
	 */
	asm volatile (
		"movdqu %0, %%xmm0\n\t"
		"movdqu %1, %%xmm1\n\t"
		"movdqu %2, %%xmm2\n\t"
		"movdqu %3, %%xmm3\n\t"
		"movdqu %4, %%xmm4\n\t"
		"movdqu %5, %%xmm5\n\t"
		"movdqu %6, %%xmm6\n\t"
		"movdqu %7, %%xmm7\n\t"
		"pxor   %%xmm1, %%xmm0\n\t"
		"aesenc %%xmm2, %%xmm0\n\t"
		"aesenc %%xmm3, %%xmm0\n\t"
		"aesenc %%xmm4, %%xmm0\n\t"
		"aesenc %%xmm5, %%xmm0\n\t"
		"aesenc %%xmm6, %%xmm0\n\t"
		"aesenc %%xmm7, %%xmm0\n\t"
		"movdqu %8, %%xmm1\n\t"
		"movdqu %9, %%xmm2\n\t"
		"movdqu %10, %%xmm3\n\t"
		"aesenc %%xmm1, %%xmm0\n\t"
		"aesenc %%xmm2, %%xmm0\n\t"
		"aesenc %%xmm3, %%xmm0\n\t"
		:
		: "m" (in[0]), "m" (aes_key[0]), "m" (aes_key[16]),
		"m" (aes_key[32]), "m" (aes_key[48]), "m" (aes_key[64]),
		"m" (aes_key[80]), "m" (aes_key[96]), "m" (aes_key[112]),
		"m" (aes_key[128]), "m" (aes_key[144])
	);
	final_index = 160;

	if (key->rounds > 10) {
		final_index = 192;
		asm volatile (
			"movdqu %0, %%xmm4\n\t"
			"movdqu %1, %%xmm5\n\t"
			"aesenc %%xmm4, %%xmm0\n\t"
			"aesenc %%xmm5, %%xmm0\n\t"
			:
			: "m" (aes_key[160]), "m" (aes_key[176])
		);

		if (key->rounds > 12) {
			asm volatile (
				"movdqu %0, %%xmm6\n\t"
				"movdqu %1, %%xmm7\n\t"
				"aesenc %%xmm6, %%xmm0\n\t"
				"aesenc %%xmm7, %%xmm0\n\t"
				:
				: "m" (aes_key[192]), "m" (aes_key[208])
			);
			final_index = 224;
		}
	}

	asm volatile (
		"movdqu %1, %%xmm1\n\t"
		"aesenclast %%xmm1, %%xmm0\n\t"
		"movdqu %%xmm0, %0\n\t"
		: "=m" (aes_state)
		: "m" (aes_key[final_index])
	);

	memcpy(out, &aes_state, sizeof(aes_state));

#endif
}
