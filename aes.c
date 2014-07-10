#include <stdio.h>

#include "aes.h"

static void aes_encrypt_block_openssl(const unsigned char *, unsigned char *, const AES_KEY *);
static void aes_encrypt_block_aesni(const unsigned char *, unsigned char *, const AES_KEY *);

void
aes_init(void)
{
	aes_encrypt_block = aes_encrypt_block_aesni;
	aes_encrypt_block = aes_encrypt_block_openssl;
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
