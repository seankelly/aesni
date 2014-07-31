int cpusupport_x86_aesni_present;
int cpusupport_x86_aesni_init;
int cpusupport_x86_aesni_detect(void);

static inline int
cpusupport_x86_aesni(void)
{
	if (cpusupport_x86_aesni_present)
		return (1);
	else if (cpusupport_x86_aesni_init)
		return (0);
	cpusupport_x86_aesni_present = cpusupport_x86_aesni_detect();
	cpusupport_x86_aesni_init = 1;
	return (cpusupport_x86_aesni_present);
}
