#include <cpuid.h>
#include <stdio.h>

#include "cpusupport.h"

#define CPUID_EAX 1
#define CPUID_ECX_BIT (0x2000000)

int
cpusupport_x86_aesni_detect(void)
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
