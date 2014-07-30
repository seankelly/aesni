#include <cpuid.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wmmintrin.h>

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
	int final_index;
	const __m128i *aes_key;
	__m128i aes_state;

	aes_key = (const __m128i *)key->rd_key;

	aes_state = _mm_loadu_si128((__m128i *)in);
	aes_state = _mm_xor_si128(aes_state, aes_key[0]);
	aes_state = _mm_aesenc_si128(aes_state, aes_key[1]);
	aes_state = _mm_aesenc_si128(aes_state, aes_key[2]);
	aes_state = _mm_aesenc_si128(aes_state, aes_key[3]);
	aes_state = _mm_aesenc_si128(aes_state, aes_key[4]);
	aes_state = _mm_aesenc_si128(aes_state, aes_key[5]);
	aes_state = _mm_aesenc_si128(aes_state, aes_key[6]);
	aes_state = _mm_aesenc_si128(aes_state, aes_key[7]);
	aes_state = _mm_aesenc_si128(aes_state, aes_key[8]);
	aes_state = _mm_aesenc_si128(aes_state, aes_key[9]);
	final_index = 10;
	if (key->rounds > 10) {
		aes_state = _mm_aesenc_si128(aes_state, aes_key[10]);
		aes_state = _mm_aesenc_si128(aes_state, aes_key[11]);
		final_index = 12;

		if (key->rounds > 12) {
			aes_state = _mm_aesenc_si128(aes_state, aes_key[12]);
			aes_state = _mm_aesenc_si128(aes_state, aes_key[13]);
			final_index = 14;
		}
	}

	aes_state = _mm_aesenclast_si128(aes_state, aes_key[final_index]);
	_mm_storeu_si128((__m128i *)out, aes_state);
}
