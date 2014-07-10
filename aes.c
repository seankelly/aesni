#include <stdio.h>

#include "aes.h"

static void aes_encrypt_block_openssl(const unsigned char *, unsigned char *, const AES_KEY *);
static void aes_encrypt_block_aesni(const unsigned char *, unsigned char *, const AES_KEY *);

int
use_aesni()
{
	/* I only have 64 bit systems on which to test. */
#ifdef __x86_64__
	return -1;
#else
	return -1;
#endif
}

void
aes_encrypt_block(const unsigned char *in, unsigned char *out, const AES_KEY *key)
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
aes_encrypt_block_openssl(const unsigned char *in, unsigned char *out, const AES_KEY *key)
{
	AES_encrypt(in, out, key);
}

static void
aes_encrypt_block_aesni(const unsigned char *in, unsigned char *out, const AES_KEY *key)
{
	AES_encrypt(in, out, key);
}
