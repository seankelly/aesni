OPENSSL_LIBS = -lcrypto
CFLAGS = -DDEBUG -DCPUSUPPORT_X86_AESNI -O0 -g -Wall -Wextra -maes
LDFLAGS = $(OPENSSL_LIBS)

OBJECT_FILES = aes.o main.o \
	       cpusupport_x86_aesni.o cpusupport_x86_aesni_detect.o


aes: $(OBJECT_FILES)
	$(CC) $(LDFLAGS) $(OBJECT_FILES) -o $@

clean:
	rm -f $(OBJECT_FILES) aes

main.o: aes.h
aes.o: cpusupport.h
cpusupport_x86_aesni_detect.o: cpusupport.h
