OPENSSL_LIBS = -lcrypto
CFLAGS = -DDEBUG -DCPUSUPPORT_X86_CPUID -DCPUSUPPORT_X86_AESNI -O2 -g -Wall -Wextra
LDFLAGS = $(OPENSSL_LIBS)

OBJECT_FILES = aes.o main.o \
	       aes_aesni.o cpusupport_x86_aesni.o


aes: $(OBJECT_FILES)
	$(CC) $(LDFLAGS) $(OBJECT_FILES) -o $@

clean:
	rm -f $(OBJECT_FILES) aes

main.o: aes.h
aes.o: cpusupport.h
cpusupport_x86_aesni.o: cpusupport.h
aes_aesni.o: CFLAGS += -maes
