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
	int final_index = 10;
	long long aes_state;
	long long aes_key[4 * (AES_MAXNR + 1)];

	memcpy(&aes_state, in, sizeof(aes_state));
	memcpy(aes_key, key->rd_key, sizeof(aes_key));

	asm volatile ("pxor   %0, %1" : "=x" (aes_state) : "x" (aes_key[0]));
	asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[1]));
	asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[2]));
	asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[3]));
	asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[4]));
	asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[5]));
	asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[6]));
	asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[7]));
	asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[8]));
	asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[9]));

	if (key->rounds > 10)
	{
		final_index = 12;
		asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[10]));
		asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[11]));
		if (key->rounds > 12)
		{
			final_index = 14;
			asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[12]));
			asm volatile ("aesenc %0, %1" : "=x" (aes_state) : "x" (aes_key[13]));
		}
	}

	asm volatile ("aesenclast %0, %1" : "=x" (aes_state) : "x" (aes_key[final_index]));

	memcpy(out, &aes_state, sizeof(aes_state));

#endif
}
