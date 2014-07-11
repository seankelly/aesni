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
	AES_encrypt(in, out, key);
}
