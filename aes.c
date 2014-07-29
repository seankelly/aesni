#include <cpuid.h>
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
	unsigned int a, b, c, d;
	if (__get_cpuid(CPUID_EAX, &a, &b, &c, &d) == 0) {
		/* Failure. */
		return -1;
	}

#ifdef DEBUG
	if ((c & bit_AES)) {
		printf("AES-NI enabled\n");
	}
#endif

	return (c & bit_AES) ? 1 : -1;
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
	const uint32_t *aes_key;

	aes_key = key->rd_key;

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
		"aesenc %%xmm3, %%xmm0"
		:
		: "m" (in[0]), "m" (aes_key[0]), "m" (aes_key[4]),
		"m" (aes_key[8]), "m" (aes_key[12]), "m" (aes_key[16]),
		"m" (aes_key[20]), "m" (aes_key[24]), "m" (aes_key[28]),
		"m" (aes_key[32]), "m" (aes_key[36])
	);
	final_index = 40;

	if (key->rounds > 10) {
		final_index = 48;
		asm volatile (
			"movdqu %0, %%xmm4\n\t"
			"movdqu %1, %%xmm5\n\t"
			"aesenc %%xmm4, %%xmm0\n\t"
			"aesenc %%xmm5, %%xmm0"
			:
			: "m" (aes_key[40]), "m" (aes_key[44])
		);

		if (key->rounds > 12) {
			asm volatile (
				"movdqu %0, %%xmm6\n\t"
				"movdqu %1, %%xmm7\n\t"
				"aesenc %%xmm6, %%xmm0\n\t"
				"aesenc %%xmm7, %%xmm0"
				:
				: "m" (aes_key[48]), "m" (aes_key[52])
			);
			final_index = 56;
		}
	}

	asm volatile (
		"movdqu %1, %%xmm1\n\t"
		"aesenclast %%xmm1, %%xmm0\n\t"
		"movdqu %%xmm0, %0"
		: "=m" (*out)
		: "m" (aes_key[final_index])
	);

#endif
}
