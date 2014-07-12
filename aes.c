#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"

static void aes_encrypt_block_openssl(const uint8_t *, uint8_t *, const AES_KEY *);
static void aes_encrypt_block_aesni(const uint8_t *, uint8_t *, const AES_KEY *);

#define CPUID_EAX 1
#define CPUID_ECX_BIT (0x2000000)

int
use_aesni()
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
	if ((c & CPUID_ECX_BIT))
	{
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

	if (aesni == 0)
	{
		aesni = use_aesni();
	}

	if (aesni == 1)
	{
		aes_encrypt_block_aesni(in, out, key);
	}
	else
	{
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

	asm volatile ("movdqu %1, %0" : "=x" (aes_state) : "m" (in[0]));
	asm volatile ("pxor   %1, %0" : "=x" (aes_state) : "m" (aes_key[0]));
	asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[16]));
	asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[32]));
	asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[48]));
	asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[64]));
	asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[80]));
	asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[96]));
	asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[112]));
	asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[128]));
	asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[144]));
	final_index = 160;

	if (key->rounds > 10)
	{
		asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[160]));
		asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[176]));
		final_index = 192;

		if (key->rounds > 12)
		{
			asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[192]));
			asm volatile ("aesenc %1, %0" : "=x" (aes_state) : "m" (aes_key[208]));
			final_index = 224;
		}
	}

	asm volatile ("aesenclast %1, %0" : "=x" (aes_state) : "m" (aes_key[final_index]));

	memcpy(out, &aes_state, sizeof(aes_state));

#endif
}
