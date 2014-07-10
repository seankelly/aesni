#include <openssl/aes.h>

void aes_init(void);
void (*aes_encrypt_block)(const unsigned char *, unsigned char *, const AES_KEY *);
