#include <stdio.h>

#include <openssl/aes.h>

extern void (*aes_encrypt_block)(const unsigned char *, unsigned char *, unsigned char *);

static void aes_encrypt_block_openssl(const unsigned char *, unsigned char *, unsigned char *);
static void aes_encrypt_block_aesni(const unsigned char *, unsigned char *, unsigned char *);

void aes_init(void)
{
    aes_encrypt_block = aes_encrypt_block_openssl;
}

static void aes_encrypt_block_openssl(const unsigned char *in, unsigned char *out, unsigned char *key)
{
    printf("openssl\n");
}

static void aes_encrypt_block_aesni(const unsigned char *in, unsigned char *out, unsigned char *key)
{
    printf("aesni\n");
}
